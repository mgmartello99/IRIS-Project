"""
pipeline:
- Start digitizer
- Acquire data for set amount of time
- Open all needed data:
    - spectrum
    - live time
    - energy calibration function
    - absolute efficiency function
    - isotope Z and A
- Perform energy calibration
- Find peaks
- Perform Gaussian Integrals
- Find number of events (Gaussian Area)
- Find branching ratio via API
- Calculate activity
- Save output data
"""
import time, json, re, sys
import numpy as np
from scipy.signal import find_peaks
from scipy.optimize import curve_fit
import requests
import uncertainties
from io import StringIO
import pandas as pd
from datetime import datetime
import matplotlib.pyplot as plt
from pathlib import Path
import joblib


# -------- CONFIGURATION --------
REMOTE_IP = "10.23.14.8" # ip address (computer running compass)
DIGITIZER_PORT = 8080 # port, default 8080
ENERGY_INTERVAL = 5 # kev, interval for peak search
ISOTOPE = "152eu" # isotope name
ACQ_TIME = 60
MODEL_PATH = "modello_leggero.pkl" # path to IsoID model
MODEL = None
IRIS_GUI_VAR_PATH = '/home/isolpharm/CSS/IRIS-GUI-latest/CompassVar/Compass.log'


# -------- import json --------
def import_parameters_json(section):
    with open("parameters.json", "r") as f:
        parameters_json = json.load(f)
    return list(parameters_json[section].values())


# -------- utilities: gaussian fit e integral --------
def gaussian(x, A, mu, sigma, bg_m, bg_q):
    return A * np.exp(-0.5 * ((x - mu) / sigma) ** 2) + bg_m * x + bg_q

def integrate_gaussian(A, sigma):
    energy_calibration_parameters = import_parameters_json("energy_calibration")
    sigma = sigma * energy_calibration_parameters[0]
    return A * np.abs(sigma) * np.sqrt(2*np.pi)


# -------- Measurement --------
def start_acquisition(acq_time, file_name="test"):
    # urls
    url_start = f"http://{REMOTE_IP}:{DIGITIZER_PORT}/compass/START"
    url_stop = f"http://{REMOTE_IP}:{DIGITIZER_PORT}/compass/STOP"
    url_runId = f"http://{REMOTE_IP}:{DIGITIZER_PORT}/compass?runId={file_name}"
    url_timedRunEnabled = f"http://{REMOTE_IP}:{DIGITIZER_PORT}/compass?timedRunEnabled=TRUE"
    url_timedRunDuration = f"http://{REMOTE_IP}:{DIGITIZER_PORT}/compass?timedRunDuration={acq_time}"
    #check for ongoing acquisition and stop it
    try:
        requests.head(url_timedRunEnabled, timeout=5)  # HEAD è più leggero di GET
    except Exception as e:
        print(f"Communication error with digitizer: {e}")
        sys.exit()
    # start acquisition
    for i in [url_stop, url_runId, url_timedRunEnabled, url_timedRunDuration]:
        requests.get(i)
        time.sleep(1) # wait 1 second between commands
    requests.get(url_start)
    time.sleep(acq_time+5) # wait acq_time + 5 seconds


# -------- Calibration --------
def calibration(channels):
    energy_calibration_parameters = import_parameters_json("energy_calibration")
    if len(energy_calibration_parameters) == 2:
        return (channels - energy_calibration_parameters[1]) / energy_calibration_parameters[0]
    else: # error
        raise ValueError("Calibration parameters must be 2 (linear) or 3 (quadratic).")


# -------- Efficiency --------
def efficiency(energy):
    p0, p1, p2, p3, p4 = import_parameters_json("efficiency")
    lnE = np.log(energy)
    eff = np.exp(p0 + p1*lnE + p2*lnE**2 + p3*lnE**3 + p4*lnE**4)
    return eff


# -------- Acquiring Branching Ratios --------
def get_decay_radiations(element):
    isotope_path = Path("isotope/"+str(element)+".csv")
    if isotope_path.exists:
        df = pd.read_csv(isotope_path)
        return df

    base_url = "https://nds.iaea.org/relnsd/v1/data"
    
    params = {
        "fields": "decay_rads",
        "nuclides": element,
        "rad_types": "g"
    }
    
    headers = {
        "User-Agent": "Mozilla/5.0 (compatible; python-requests example)"
    }
    try:
        resp = requests.get(base_url, params=params, headers=headers)
        resp.raise_for_status()
        
        csv_text = resp.text
        
        # Use StringIO to read CSV text
        df = pd.read_csv(StringIO(csv_text))
        
        return df
    except Exception as e:
        print(f"Error fetching data for isotope {element}: {e}")
        sys.exit()

def get_branching_ratio(energy_measured):
    br_vec = []
    if Path(f"database/{ISOTOPE}.csv").exists():
        df = pd.read_csv(f"database/{ISOTOPE}.csv")
        for _, row in df.iterrows():
            energy = row.get("energy")      # keV
            intensity = row.get("intensity")  # %
            if energy> energy_measured - ENERGY_INTERVAL and energy < energy_measured + ENERGY_INTERVAL:
                br_vec.append(intensity / 100) # convert to fraction
    else:
        df = get_decay_radiations(ISOTOPE)
        for _, row in df.iterrows():
            energy = row.get("energy")      # keV
            intensity = row.get("intensity")  # %
            parent_energy = row.get("p_energy")  # keV
            if parent_energy == 0 and energy> energy_measured - ENERGY_INTERVAL and energy < energy_measured + ENERGY_INTERVAL:
                br_vec.append(intensity / 100) # convert to fraction
        try:
            df2 = df.loc[df['p_energy'] == 0, ['energy', 'intensity']]
            df2.to_csv(f"database/{ISOTOPE}.csv", index=False, header=True)
        except Exception as e:
            print(f"Could not save branching ratio data for isotope {ISOTOPE}. Error: {e}")
            
    if len(br_vec) == 0:
        print(f"Warning: No branching ratio found for energy {energy_measured} keV in isotope {ISOTOPE}. Setting BR=1.")
        return 1
    return np.max(br_vec)


# -------- Importing Data --------
def get_live_time_seconds(file_path):
    with open(file_path, "r") as f:
        for line in f:
            if "Live time" in line:
                # cerca un tempo nel formato H:MM:SS.mmm
                match = re.search(r"(\d+):(\d+):(\d+\.\d+)", line)
                if match:
                    hours = int(match.group(1))
                    minutes = int(match.group(2))
                    seconds = float(match.group(3))
                    total_seconds = hours * 3600 + minutes * 60 + seconds
                    return total_seconds
    return None

def import_spectrum(path):
    spectrum = pd.read_csv(next((Path(path) / 'RAW').glob('*.txt'))).to_numpy().flatten()
    live_time = get_live_time_seconds(next((Path(path)).glob('*_info.txt')))
    normalized_spectrum = spectrum / live_time
    return normalized_spectrum


# -------- Finding Peaks --------
def find_peaks_spectrum(spectrum):
    x_kev = calibration(np.arange(len(spectrum)))
    peaks, _ = find_peaks(spectrum, distance=100, prominence=.6)
    return peaks  # return indices of peaks


# -------- Analysis --------
def analyze_spectrum():
    spectrum = import_spectrum(PATH)
    calibrated_x_axis = calibration(np.arange(len(spectrum)))
    peaks = find_peaks_spectrum(spectrum)
    plt.plot(calibrated_x_axis, spectrum)
    plt.scatter(calibrated_x_axis[peaks], spectrum[peaks], color='red', marker="x")
    plt.xlabel("Energy (keV)")
    plt.ylabel("Counts")

    means = []
    area = []
    rangevar = 25
    for peak in peaks:
        popt, pcov = curve_fit(gaussian, calibrated_x_axis[peak-rangevar:peak+rangevar], spectrum[peak-rangevar:peak+rangevar], p0=[spectrum[peak], calibrated_x_axis[peak], 1, 0, 0])
        A, mu, sigma, bg_m, bg_q = uncertainties.correlated_values(popt, pcov)
        means.append(mu)
        area.append(integrate_gaussian(A, sigma))
        plt.plot(calibrated_x_axis[peak-rangevar:peak+rangevar], gaussian(calibrated_x_axis[peak-rangevar:peak+rangevar], *popt))
    plt.show()
    return means, area

def calculate_activity():
    means, areas = analyze_spectrum()
    activities = []
    for mu, A in zip(means, areas):
        br = get_branching_ratio(mu.n)
        eff = efficiency(mu.n)
        activity = A / (br * eff)
        activities.append(activity)
    return activities


# -------- Isotope Identification --------
def spectrum_to_bins_IsoID(peaks, bin_edges=np.arange(0, 3001, 5)):
    hist, _ = np.histogram(peaks, bins=bin_edges)
    return hist

def import_IsoID_model(model_path=MODEL_PATH):
    model = joblib.load(model_path)
    return model

def IsoID(peaks):
    binned_spectrum = spectrum_to_bins_IsoID(peaks).reshape(1, -1)
    prediction = MODEL.predict(binned_spectrum)
    print(f'Prediction: {prediction}')
    return prediction[0]


# -------- Average Activity --------
def calculate_average(activities):
    n, d = 0, 0
    for i in activities:
        n += i.n / (i.s)**2
        d += 1 / (i.s)**2
    return n / d, np.sqrt(1 / d)


# -------- Import GUI Data --------
def import_GUI_data():
    gui_data = pd.read_csv(IRIS_GUI_VAR_PATH, header=None, sep=r'\s{2,}').to_numpy()
    global ISOTOPE
    global ACQ_TIME
    ISOTOPE = gui_data[1,1]
    ACQ_TIME = int(gui_data[0,1])


# -------- Main Pipeline --------
if __name__ == "__main__":
    # MODEL = import_IsoID_model() # IID to be fixed
    import_GUI_data()
    runId = datetime.now().strftime("%Y%m%d_%H%M%S")
    PATH = "/home/isolpharm/remote/DAQ/"+runId
    start_acquisition(ACQ_TIME, file_name=runId)
    activities = calculate_activity()
    
    # for debugging
    for i in activities:
        print("Calculated activities (Bq): ", i)
    
    avr_activity = calculate_average(activities)
    print("Averaged activity: ",avr_activity)
TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = iris
# iris.dbd will be created and installed
DBD += iris.dbd

# iris.dbd will be made up from these files:
iris_DBD += base.dbd

iris_DBD += asyn.dbd
iris_DBD += drvAsynIPPort.dbd
iris_DBD += modbusSupport.dbd
iris_DBD += asSupport.dbd

# Include dbd files from all support applications:
#iris_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#iris_LIBS += xxx

# iris_registerRecordDeviceDriver.cpp derives from iris.dbd
iris_SRCS += iris_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
iris_SRCS_DEFAULT += irisMain.cpp
iris_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#iris_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
iris_LIBS += $(EPICS_BASE_IOC_LIBS)

iris_LIBS += asyn
iris_LIBS += modbus
iris_LIBS += autosave

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


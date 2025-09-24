#!../../bin/linux-x86_64/iris

#- You may have to change iris to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/iris.dbd"
iris_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/iris.db","user=spes")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=spes"

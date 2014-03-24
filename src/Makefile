TOP=..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# Build an IOC support library
#

DBD += pvaAsyn.dbd

INC += pvaAsyn.h
INC += asynInterfaceV4.h

LIBRARY_IOC += pvaAsyn
pvaAsyn_SRCS += pvaAsyn.cpp
pvaAsyn_SRCS += pvaAsynRegister.cpp
pvaAsyn_SRCS += asynInterfaceV4.cpp
pvaAsyn_LIBS += pvData
pvaAsyn_LIBS += pvAccess
pvaAsyn_LIBS += pvDatabase
pvaAsyn_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


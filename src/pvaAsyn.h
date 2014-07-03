/* pvaAsyn.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2013.07.24
 */
#ifndef PVAASYN_H
#define PVAASYN_H

#include <shareLib.h>

#include <pv/pvDatabase.h>
#include <pv/asynInterfaceV4.h>

namespace epics { namespace pvaAsyn { 

class epicsShareClass  PvaAsyn{
public:
    static void create(const char *recordName,const char *portName,const char *attrNames);
};

class PvaAsynRecord;
typedef std::tr1::shared_ptr<PvaAsynRecord> PvaAsynRecordPtr;

class epicsShareClass PvaAsynRecord :
    public epics::pvDatabase::PVRecord
{
public:
    POINTER_DEFINITIONS(PvaAsynRecord);
    static PvaAsynRecordPtr create(
        std::string const & recordName,
        epics::pvData::PVStructurePtr const & pvStructure,
        epics::pvaAsyn::AsynInterfaceV4Ptr const & asynInterface);
    virtual ~PvaAsynRecord();
    virtual void destroy();
    virtual bool init();
    virtual void process();
private:
    PvaAsynRecord(std::string const & recordName,
        epics::pvData::PVStructurePtr const & pvTop,
        epics::pvaAsyn::AsynInterfaceV4Ptr const & asynInterface);

    epics::pvaAsyn::AsynInterfaceV4Ptr asynInterface;
    epics::pvData::PVStringArrayPtr pvName;
    epics::pvData::PVStringArrayPtr pvValue;
    epics::pvData::PVStringArrayPtr pvType;
};


}}

#endif  /* PVAASYN_H */

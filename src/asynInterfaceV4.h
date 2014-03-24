/* asynInterfaceV4.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2014.02.18
 */
#ifndef ASYNINTERFACEV4_H
#define ASYNINTERFACEV4_H

#include <shareLib.h>
#include <pv/event.h>
#include <pv/pvDatabase.h>

#include <asynDriver.h>


namespace epics { namespace pvaAsyn { 

class AsynInterfaceV4;
typedef std::tr1::shared_ptr<AsynInterfaceV4> AsynInterfaceV4Ptr;

class AsynInterface;
typedef std::tr1::shared_ptr<AsynInterface> AsynInterfacePtr;

class epicsShareClass  AsynInterfaceV4
: public std::tr1::enable_shared_from_this<AsynInterfaceV4>
{
public:
    POINTER_DEFINITIONS(AsynInterfaceV4);
    ~AsynInterfaceV4();
    static AsynInterfaceV4Ptr create (
        epics::pvData::String const &portName,
        epics::pvData::String const &attrNames);
    void getAttr();
    void putAttr();
    epics::pvData::shared_vector<const epics::pvData::String> getName();
    epics::pvData::shared_vector<const epics::pvData::String> getValue();
    epics::pvData::shared_vector<const epics::pvData::String> getType();
    void put(epics::pvData::shared_vector<const epics::pvData::String> const &value);
    void getCallback();
    void putCallback();
private:
    AsynInterfaceV4Ptr getPtrSelf()
    {
        return shared_from_this();
    }
    AsynInterfaceV4(epics::pvData::String const &portName);
    bool init(epics::pvData::String const &attrNames);
    epics::pvData::String portName;
    epics::pvData::shared_vector<epics::pvData::String> attrName;
    epics::pvData::shared_vector<epics::pvData::String> attrValue;
    epics::pvData::shared_vector<epics::pvData::String> attrType;
    epics::pvData::shared_vector<epicsInt32> intValue;
    epics::pvData::shared_vector<epicsFloat64> doubleValue;
    epics::pvData::shared_vector<epics::pvData::String> stringValue;
    epics::pvData::shared_vector<AsynInterfacePtr> asynInterface;
    epics::pvData::Event event;
    asynUser *asynUserGet;
    asynUser *asynUserPut;
};


}}

#endif  /* ASYNINTERFACEV4_H */

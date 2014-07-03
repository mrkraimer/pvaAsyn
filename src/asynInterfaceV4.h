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
        std::string const &portName,
        std::string const &attrNames);
    void getAttr();
    void putAttr();
    epics::pvData::shared_vector<const std::string> getName();
    epics::pvData::shared_vector<const std::string> getValue();
    epics::pvData::shared_vector<const std::string> getType();
    void put(epics::pvData::shared_vector<const std::string> const &value);
    void getCallback();
    void putCallback();
private:
    AsynInterfaceV4Ptr getPtrSelf()
    {
        return shared_from_this();
    }
    AsynInterfaceV4(std::string const &portName);
    bool init(std::string const &attrNames);
    std::string portName;
    epics::pvData::shared_vector<std::string> attrName;
    epics::pvData::shared_vector<std::string> attrValue;
    epics::pvData::shared_vector<std::string> attrType;
    epics::pvData::shared_vector<epicsInt32> intValue;
    epics::pvData::shared_vector<epicsFloat64> doubleValue;
    epics::pvData::shared_vector<std::string> stringValue;
    epics::pvData::shared_vector<AsynInterfacePtr> asynInterface;
    epics::pvData::Event event;
    asynUser *asynUserGet;
    asynUser *asynUserPut;
};


}}

#endif  /* ASYNINTERFACEV4_H */

/*pvaAsyn.cpp */

/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2013.07.24
 */

/* Author: Marty Kraimer */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include <pv/pvDatabase.h>
#include <pv/pvData.h>
#include <pv/channelProviderLocal.h>
#include <pv/recordList.h>
#include <pv/traceRecord.h>
#include <pv/pvaAsyn.h>
#include <pv/asynInterfaceV4.h>

//namespace epics { namespace pvaAsyn {

using std::cout;
using std::endl;
using std::tr1::static_pointer_cast;
using namespace epics::pvData;
using namespace epics::pvAccess;

using namespace epics::pvDatabase;
using namespace epics::pvaAsyn;

void PvaAsyn::create(
    const char * record,
    const char *port,
    const char *attr)
{
    if(record==NULL) {
        cout<< "recordName is null" << endl;
        return;
    }
    if(port==NULL) {
        cout<< "portName is null" << endl;
        return;
    }
    if(attr==NULL) {
        cout<< "attrNames is null" << endl;
        return;
    }
    String recordName(record);
    String portName(port);
    String attrName(attr);
    AsynInterfaceV4Ptr asynInterface(AsynInterfaceV4::create(portName,attr));
    if(asynInterface==NULL) return;
    asynInterface->getAttr();
    shared_vector<const String> nameValue
        = asynInterface->getName();
    shared_vector<const String> valueValue
        = asynInterface->getValue();
    shared_vector<const String> typeValue
        = asynInterface->getType();
    FieldCreatePtr fieldCreate = getFieldCreate();
    PVDataCreatePtr pvDataCreate = getPVDataCreate();
    StringArray name(3);
    name[0] = "name";
    name[1] = "value";
    name[2] = "type";
    FieldConstPtrArray field(3);
    field[0] = fieldCreate->createScalarArray(pvString);
    field[1] = fieldCreate->createScalarArray(pvString);
    field[2] = fieldCreate->createScalarArray(pvString);
    StructureConstPtr struct1 = fieldCreate->createStructure(name,field);
    PVStructurePtr pvStructure = pvDataCreate->createPVStructure(struct1);
    PVStringArrayPtr pvStringArray = std::tr1::static_pointer_cast<PVStringArray>
        (pvStructure->getSubField("name"));
    pvStringArray->replace(nameValue);
    pvStringArray = std::tr1::static_pointer_cast<PVStringArray>
        (pvStructure->getSubField("value"));
    pvStringArray->replace(valueValue);
    pvStringArray = std::tr1::static_pointer_cast<PVStringArray>
        (pvStructure->getSubField("type"));
    pvStringArray->replace(typeValue);
    PVDatabasePtr master = PVDatabase::getMaster();
    PVRecordPtr pvRecord(PvaAsynRecord::create(recordName,pvStructure,asynInterface));
    bool result = PVDatabase::getMaster()->addRecord(pvRecord);
    if(!result) cout << "recordname" << " not added" << endl;
}

PvaAsynRecordPtr PvaAsynRecord::create(
        String const & recordName,
        PVStructurePtr const & pvStructure,
        AsynInterfaceV4Ptr const & asynInterface)
{
    PvaAsynRecordPtr pvRecord(new PvaAsynRecord(recordName,pvStructure,asynInterface));
    if(!pvRecord->init()) pvRecord.reset();
    return pvRecord;
}

PvaAsynRecord::~PvaAsynRecord()
{
}

void PvaAsynRecord::destroy()
{
    PVRecord::destroy();
}

bool PvaAsynRecord::init()
{
    initPVRecord();
    PVFieldPtr pvField;
    pvField = getPVStructure()->getSubField("name");
    if(pvField==NULL) return false;
    pvName = static_pointer_cast<PVStringArray>(pvField);
    pvField = getPVStructure()->getSubField("value");
    if(pvField==NULL) return false;
    pvValue = static_pointer_cast<PVStringArray>(pvField);
    return true;
}

void PvaAsynRecord::process()
{
    asynInterface->put(pvValue->view());
    asynInterface->putAttr();
    if(pvValue->getLength()!=pvName->getLength()) {
        pvValue->replace(asynInterface->getValue());
    }
}

PvaAsynRecord::PvaAsynRecord(String const & recordName,
        PVStructurePtr const & pvTop,
        AsynInterfaceV4Ptr const & asynInterface)
: PVRecord(recordName,pvTop),
  asynInterface(asynInterface)
{
}

//}}

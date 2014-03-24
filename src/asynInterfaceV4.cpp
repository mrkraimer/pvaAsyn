/* asynInterfaceV4.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2014.02.18
 */

#include <sstream>

#include <pv/asynInterfaceV4.h>
#include <asynDrvUser.h>
#include <asynInt32.h>
#include <asynFloat64.h>
#include <asynOctet.h>
#include <asynDrvUser.h>

namespace epics { namespace pvaAsyn { 
using std::tr1::static_pointer_cast;
using std::string;
using std::cout;
using std::endl;
using namespace epics::pvData;

enum InterfaceType { typeInt, typeDouble, typeString };

class AsynInterface {
public:
    asynUser *pasynUser;
    InterfaceType type;
    asynFloat64 * pasynFloat64;
    asynInt32 * pasynInt32;
    asynOctet * pasynOctet;
    void *drvPvt;
};

extern "C" void AsynV4Callback(asynUser *pasynUser)
{
    AsynInterfaceV4 * xxx = static_cast<AsynInterfaceV4 *>(pasynUser->userPvt);
    xxx->getCallback();
}

extern "C" void AsynV4GetCallback(asynUser *pasynUser)
{
    AsynInterfaceV4 * xxx = static_cast<AsynInterfaceV4 *>(pasynUser->userPvt);
    xxx->getCallback();
}

extern "C" void AsynV4PutCallback(asynUser *pasynUser)
{
    AsynInterfaceV4 * xxx = static_cast<AsynInterfaceV4 *>(pasynUser->userPvt);
    xxx->putCallback();
}

AsynInterfaceV4Ptr AsynInterfaceV4::create (
    String const &portName,String const &attrNames)
{
    AsynInterfaceV4Ptr xxx(
        new AsynInterfaceV4(portName));
    if(!xxx->init(attrNames)) xxx.reset();
    return xxx;
}

AsynInterfaceV4::AsynInterfaceV4 (
    String const &portName)
: portName(portName),
  asynUserGet(NULL),
  asynUserPut(NULL)
{
}

AsynInterfaceV4::~AsynInterfaceV4()
{
    if(asynUserGet!=NULL) pasynManager->freeAsynUser(asynUserGet);
    asynUserGet = NULL;
    if(asynUserPut!=NULL) pasynManager->freeAsynUser(asynUserPut);
    asynUserPut = NULL;
    size_t n = asynInterface.size();
    for(size_t i=0; i<n; ++i) {
        AsynInterfacePtr interface = asynInterface[i];
        if(interface->pasynUser!=NULL) {
            asynStatus status = pasynManager->freeAsynUser(interface->pasynUser);
            if(status!=asynSuccess)
            {
                cout << "AsynInterfaceV4::~AsynInterfaceV4 " << interface->pasynUser->errorMessage << endl;
            }
            interface.reset();
        }
    }
}

bool AsynInterfaceV4::init(String const &attrNames)
{
    if(portName.empty()) {
        cout << "portName is null";
        return false;
    }
    if(attrNames.empty()) {
        cout << "attrNames is null";
        return false;
    }
    asynUserGet = pasynManager->createAsynUser(AsynV4GetCallback,0);
    asynUserGet->userPvt = this;
    asynStatus status;
    status = pasynManager->connectDevice(asynUserGet,portName.c_str(),0);
    if(status!=asynSuccess)
    {
        cout << "AsynInterfaceV4::create " << asynUserGet->errorMessage << endl;
        return false;
    }
    asynUserPut = pasynManager->createAsynUser(AsynV4PutCallback,0);
    asynUserPut->userPvt = this;
    status = pasynManager->connectDevice(asynUserPut,portName.c_str(),0);
    if(status!=asynSuccess)
    {
        cout << "AsynInterfaceV4::create "
           << asynUserPut->errorMessage << endl;
        return false;
    }
    size_t ncomma = 0;
    size_t result = 0;
    size_t next = 0;
    while(true) {
        result = attrNames.find(',',next);
        if(result==string::npos) break;
        ++ncomma;
        next = result+1;
    }
    size_t nattr = ncomma + 1;
    attrName = shared_vector<String>(nattr);
    attrValue = shared_vector<String>(nattr);
    attrType = shared_vector<String>(nattr);
    intValue = shared_vector<int>(nattr);
    doubleValue = shared_vector<double>(nattr);
    stringValue = shared_vector<String>(nattr);
    asynInterface = shared_vector<AsynInterfacePtr>(nattr);
    result = 0;
    next = 0;
    for(size_t i=0; i<nattr; ++i) {
        void       *drvPvt;
        String rest = attrNames.substr(next);
        result = rest.find(',');
        if(result!=string::npos) {
           rest = rest.substr(0,result);
           next += rest.size() +1;
        }
        attrName[i] = rest;
        asynInterface[i] = AsynInterfacePtr(new AsynInterface());
        asynUser * pasynUser = pasynManager->createAsynUser(AsynV4Callback,0);
        asynInterface[i]->pasynUser = pasynUser;
        status = pasynManager->connectDevice(pasynUser,portName.c_str(),0);
        if(status!=asynSuccess)
        {
            cout << "AsynInterfaceV4::create connectDevice  " << portName
               << pasynUser->errorMessage << endl;
            return false;
        }
        ::asynInterface *pasynDrvInterface;
        pasynDrvInterface = pasynManager->findInterface(pasynUser,asynDrvUserType,1);
        if(pasynDrvInterface==NULL)
        {
            cout << "AsynInterfaceV4::create findInterface asynDrvUserType "
               << pasynUser->errorMessage << endl;
            return false;
        }
        const char *xxx = attrName[i].c_str();
        asynDrvUser *pasynDrvUser;
        pasynDrvUser = (asynDrvUser *)pasynDrvInterface->pinterface;
        drvPvt = pasynDrvInterface->drvPvt;
        status = pasynDrvUser->create(drvPvt,pasynUser,xxx,0,0);
        if(status!=asynSuccess) {
            cout << "AsynInterfaceV4::create asynDrvUser attrName "
               << attrName[i]
               << pasynUser->errorMessage << endl;
            return false;
        }
        ::asynInterface *pasynInterface;
        pasynInterface=pasynManager->findInterface(pasynUser,asynInt32Type,1);
        if(pasynInterface!=NULL) {
            drvPvt = pasynInterface->drvPvt;
            asynInt32 * pasynInt32 = static_cast<asynInt32 *>
                (pasynInterface->pinterface);
            epicsInt32 value = -1;
            status = pasynInt32->read(drvPvt,pasynUser,&value);
            if(status!=asynSuccess) {
                 pasynInterface = NULL;
            } else {
            intValue[i] = value;
            attrType[i] = String("integer");
            asynInterface[i]->pasynInt32 = pasynInt32;
            asynInterface[i]->type = typeInt;
            }
        }
        if(pasynInterface==NULL) {
            pasynInterface = pasynManager->findInterface(pasynUser,asynOctetType,1);
            if(pasynInterface!=NULL) {
                drvPvt = pasynInterface->drvPvt;
                asynOctet * pasynOctet = static_cast<asynOctet *>
                    (pasynInterface->pinterface);

                size_t size = 80;
                size_t ntransfered = 0;
                int eom = 0;
                char value[80];
                char *pvalue = &value[0];
                status = pasynOctet->read(drvPvt,pasynUser,pvalue,size,&ntransfered,&eom);
                if(status!=asynSuccess) {
                     pasynInterface = NULL;
                } else {
                    attrType[i] = String("string");
                    stringValue[i] = String(value);
                    asynInterface[i]->pasynOctet = pasynOctet;
                    asynInterface[i]->type = typeString;
                }
            }
        }
        if(pasynInterface==NULL) {
            pasynInterface = pasynManager->findInterface(
                pasynUser, asynFloat64Type, 1);
            if(pasynInterface==NULL) {
                cout << "AsynInterfaceV4::create "
                   << pasynUser->errorMessage << endl;
                return false;
            }
            drvPvt = pasynInterface->drvPvt;
            asynFloat64 *pasynFloat64 = static_cast<asynFloat64 *>(
                pasynInterface->pinterface);
            epicsFloat64 value = -1.0;
            status = pasynFloat64->read(drvPvt,pasynUser,&value);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::getAttr "
                   << pasynUser->errorMessage << endl;
                return false;
            }
            attrType[i] = String("float");
            doubleValue[i] = value;
            asynInterface[i]->pasynFloat64 = pasynFloat64;
            asynInterface[i]->type = typeDouble;
        }
        asynInterface[i]->drvPvt = pasynInterface->drvPvt;
    }
    return true;
}

void AsynInterfaceV4::getAttr()
{
    asynStatus status;
    status = pasynManager->queueRequest(asynUserGet,asynQueuePriorityLow,0.0);
    if(status!=asynSuccess)
    {
        cout << "AsynInterfaceV4::getAttr "
           << asynUserGet->errorMessage << endl;
        return;
    }
    event.wait();
}

void AsynInterfaceV4::putAttr()
{
    asynStatus status;
    status = pasynManager->queueRequest(asynUserPut,asynQueuePriorityLow,0.0);
    if(status!=asynSuccess)
    {
        cout << "AsynInterfaceV4::putAttr "
           << asynUserPut->errorMessage << endl;
        return;
    }
    event.wait();
}

shared_vector<const String> AsynInterfaceV4::getName()
{
   shared_vector<String> copy(attrName);
   copy.make_unique();
   shared_vector<const String> data(freeze(copy));
   return data;
}

shared_vector<const String> AsynInterfaceV4::getValue()
{
   shared_vector<String> copy(attrValue);
   copy.make_unique();
   shared_vector<const String> data(freeze(copy));
   return data;
}

shared_vector<const String> AsynInterfaceV4::getType()
{
   shared_vector<String> copy(attrType);
   copy.make_unique();
   shared_vector<const String> data(freeze(copy));
   return data;
}

void AsynInterfaceV4::put(
    shared_vector<const String> const &value)
{
   size_t numAttr = attrValue.size();
   size_t num = value.size();
   if(num>numAttr) num = numAttr;
   for (size_t i=0; i< num; ++i) {
       attrValue[i] = value[i];
   }
}

void AsynInterfaceV4::getCallback()
{
   asynStatus status;
   size_t num = asynInterface.size();
   for(size_t i=0; i<num; ++i) {
        asynUser *pasynUser = asynInterface[i]->pasynUser;
        void * drvPvt = asynInterface[i]->drvPvt;
        if(asynInterface[i]->type==typeInt) {
            epicsInt32 value = -1;
            status = asynInterface[i]->pasynInt32->read(drvPvt,pasynUser,&value);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::getAttr " << pasynUser->errorMessage << endl;
            }
            std::ostringstream out;
            out << value;
            attrValue[i] = out.str();
        } else if(asynInterface[i]->type==typeString) {
            size_t size = 80;
            size_t ntransfered = 0;
            int eom;
            char value[size];
            status = asynInterface[i]->pasynOctet->read(
                drvPvt,pasynUser,value,size,&ntransfered,&eom);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::getAttr " << pasynUser->errorMessage << endl;
            } else {
                stringValue[i] = String(value);
                attrValue[i] = stringValue[i];
            }
        } else if(asynInterface[i]->type==typeDouble) {
            epicsFloat64 value = -1.0;
            status = asynInterface[i]->pasynFloat64->read(drvPvt,pasynUser,&value);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::getAttr "
                   << pasynUser->errorMessage << endl;
            }
            std::ostringstream out;
            out << value;
            attrValue[i] = out.str();
        } else {
            cout << "AsynInterfaceV4::getAttr  unsupported type" << endl;
        }
   }
   event.signal();
}


void AsynInterfaceV4::putCallback()
{
   asynStatus status;
   size_t num = asynInterface.size();
   for(size_t i=0; i<num; ++i) {
        asynUser *pasynUser = asynInterface[i]->pasynUser;
        void * drvPvt = asynInterface[i]->drvPvt;
        if(asynInterface[i]->type==typeInt) {
            epicsInt32 value;
            std::istringstream in(attrValue[i]);
            in >> value;
            status = asynInterface[i]->pasynInt32->write(drvPvt,pasynUser,value);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::putAttr "
                   << pasynUser->errorMessage << endl;
            }
        } else if(asynInterface[i]->type==typeString) {
            stringValue[i] = attrValue[i];
            char * value = const_cast<char *>(stringValue[i].c_str());
            size_t nbytesTransfered;
            status = asynInterface[i]->pasynOctet->write(drvPvt,pasynUser,
                value,stringValue[i].size(),&nbytesTransfered);
        } else if(asynInterface[i]->type==typeDouble) {
            epicsFloat64 value;
            std::istringstream in(attrValue[i]);
            in >> value;
            status = asynInterface[i]->pasynFloat64->write(drvPvt,pasynUser,value);
            if(status!=asynSuccess) {
                cout << "AsynInterfaceV4::getAttr "
                   << pasynUser->errorMessage << endl;
            }
        } else {
            cout << "AsynInterfaceV4::getAttr  unsupported type" << endl;
        }
   }
   event.signal();
}


}}


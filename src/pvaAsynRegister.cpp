/*pvaAsynRegister.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 * @author mrk
 * @date 2014.02.14
 */


/* Author: Marty Kraimer */

#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <memory>

#include <cantProceed.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>
#include <iocsh.h>

#include <epicsExport.h>

#include <pv/pvIntrospect.h>
#include <pv/pvData.h>
#include <pv/pvAccess.h>
#include <pv/pvDatabase.h>
#include <pv/pvaAsyn.h>

using namespace epics::pvData;
using namespace epics::pvAccess;
using namespace epics::pvDatabase;
using namespace epics::pvaAsyn;

static const iocshArg testArg0 = { "recordName", iocshArgString };
static const iocshArg testArg1 = { "portName", iocshArgString };
static const iocshArg testArg2 = { "attrNames", iocshArgString };
static const iocshArg *testArgs[] = {
    &testArg0,&testArg1,&testArg2};

static const iocshFuncDef pvaAsynFuncDef = {
    "pvaAsyn", 3 , testArgs};
static void pvaAsynCallFunc(const iocshArgBuf *args)
{
    PvaAsyn::create(args[0].sval,args[1].sval,args[2].sval);
}

static void pvaAsynRegister(void)
{
    static int firstTime = 1;
    if (firstTime) {
        firstTime = 0;
        iocshRegister(&pvaAsynFuncDef, pvaAsynCallFunc);
    }
}
epicsExportRegistrar(pvaAsynRegister);

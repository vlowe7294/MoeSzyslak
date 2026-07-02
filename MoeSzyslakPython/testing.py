from sys import version
from _pytest.recwarn import T
import MoeSzyslakPython
from MoeSzyslakPython import *
from xml.dom import minidom
from comtypes import BSTR

import time
import traceback
from typing import Type, List, Callable, Any
import datetime
import xml.etree.ElementTree as ET
import math
import UserClass
import comtypes

from UserClass import *


LOGENTRY_IID = GUID("{AC597284-A906-482F-9BE9-C4E13AB43E50}")

class ILOGENTRY(IUnknown):
    _iid_ = LOGENTRY_IID
    _methods_ = [
         COMMETHOD(
            [], HRESULT, "GetText",
            (['out, retVal'], POINTER(BSTR), "strTxt"),
        ),

         COMMETHOD(
            [], HRESULT, "SetText",
            (['in'], c_wchar_p, "strTxt"),
        ),
        
        COMMETHOD(
            [], HRESULT, "GetTime",
            (['out, retVal'], POINTER(c_longlong), "tme"),
        ),   
    ]

## @class LogEntry
## @brief Represents a single log entry containing a timestamp and text message.
##
## This class stores a time value and an associated text string. It provides
## read‑only access to the timestamp through a property.
class LogEntry:

    def __init__(self, iLogEntry): 
        print('LogEntry constructor')
        self._text = ''
       
        self._iLogEntry = iLogEntry
        
        t = c_longlong()

        self._iLogEntry.GetTime(comtypes.byref(t))
        self._nTime = t.value
        print('end constructor')

    def Print(self):
        print(f"\t{self._nTime}\t{self.text}") 

    ## @property Time
    ## @brief Gets the timestamp of the log entry.
    ##
    ## @return The stored timestamp value.
    @property
    def time(self):
        return self._time

    ## @property Text
    ## @brief Gets the text message associated with the log entry.
    ##
    ## @return The stored text string.
    @property
    def text(self, txt = None):

        if txt is not None:
            self._iLogEntry.SetText(txt)

        txt_bstr = BSTR()
        self._iLogEntry.GetText(comtypes.byref(txt_bstr))
        self._text = txt_bstr.value
        return self._text
    
TESTING_IID = GUID("{7C6DA0F8-84AE-4E97-86F0-13BB1E67C713}")

class ITESTING(IUnknown):
    _iid_ = TESTING_IID
    _methods_ = [
         COMMETHOD(
            [], HRESULT, "Message",
            (['in'], c_wchar_p, "strMsg"),
            (['in'], c_int32, "nDebugLvl"),
            (['in'], c_wchar_p, "strCategory"),
        ),

        COMMETHOD(
            [], HRESULT, "SetDebugLevel",
            (['in'], c_int32, "level"),
        ),

        COMMETHOD(
            [], HRESULT, "GetLogEntry",
            (['in'], c_int32, "ndx"),
            (['out, retval'], CPOINTER(CPOINTER(IUnknown)), "iEntry")
        ),

        COMMETHOD(
            [], HRESULT, "VerifyVariable",
            (['in'], c_wchar_p, "varName"),
            (['in'], c_wchar_p, "val"),
        ),

        COMMETHOD(
            [], HRESULT, "VerifyHResult",
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iPrp")
        ), 
        
        COMMETHOD(
            [], HRESULT, "GetTestData",
            (['in'], c_wchar_p, "szName"),
            (['out, retVal'], POINTER(BSTR), "iStr")
        ),

        COMMETHOD(
            [], HRESULT, "SetTestData",
            (['in'], c_wchar_p, "strName"),
            (['in'], c_wchar_p, "strVal")
        ),
    ]

class Testing:

    DEBUG_FULL = 0
    DEBUG_VERBOSE = 1
    DEBUG_INFO = 2
    DEBUG_WARN = 3
    DEBUG_CRITICAL = 4

    def __init__(self, unk_ptr):
        self._iunk = unk_ptr
        self._iTesting = unk_ptr.QueryInterface(ITESTING)
        self._logEntries = []
        self._debugLevel = Testing.DEBUG_FULL

    def Message(self, strMsg, nDebugLvl, strCat):
        self._iTesting.Message(strMsg, nDebugLvl, strCat)
        

    def run_test_method(self, instance: Any, method: Callable):
        """Run a single test method and measure execution time."""
        start = time.perf_counter()
        error = None

        try:
            method()
        except Exception as e:
            error = traceback.format_exc()

        end = time.perf_counter()
        duration = end - start

        return duration, error

    def RunTest(self, objToTest):

        try:
            self._logEntries = []
            
            if objToTest != type(TestRunner):
                self.Verify(False, f"No test defined for type {objToTest}")
            else:
                self.SelfTest()

        except Exception as e:
            print(f"Error running test: {e}")
            self.Message(str(e))
            self._bPassed = False

    def Verify(self, bVal, szMsg):
        if not bVal:
            self.Message(szMsg)
            self._bPassed = False

    def TestData(self, name, val = None):        
        if val is not None:
            self._iTesting.SetTestData(name, val)

        txt_bstr = BSTR()

        self._iTesting.GetTestData(name, comtypes.byref(txt_bstr))
        val = txt_bstr.value        
        return val

    def LogEntry(self, ndx):
        if 0 <= ndx < len(self._logEntries):
            return self._logEntries[ndx]
        else:
            raise IndexError("Log entry index out of range")

    def Report(self):
        print("\tStart time(ms)\tText")
        n = 1
        self.Update()

        for le in self._logEntries:
            print(str(n), end="")
            le.Print()
            n = n + 1

    def Clear(self):
        self._testValues = {}
        self._bMemoryCheck = False
        self._bPassed = True
        self._logEntries = []
        self._testTime = datetime.datetime.now()

    def Update(self):
        unk_ptr = CPOINTER(IUnknown)()
        self._iTesting.GetLogEntry(0, byref(unk_ptr))
        self._logEntries.clear()
        nCnt = 0

        while unk_ptr:
            iLogEntry = unk_ptr.QueryInterface(ILOGENTRY)
            unk_ptr = CPOINTER(IUnknown)()
            self._logEntries.append(LogEntry(iLogEntry))
            nCnt = nCnt + 1
            self._iTesting.GetLogEntry(nCnt, byref(unk_ptr))

    def VerifyVariable(self, varName, val):
        self._iTesting.VerifyVariable(varName, val)

    @property
    def DebugLevel(self):
        return self._debugLevel

    @DebugLevel.setter
    def DebugLevel(self, dbgVal):
        self._iTesting.SetDebugLevel(dbgVal)
        self._debugLevel = dbgVal

    def UnitTest(cmpste):
        tst = cmpste.theTester()
        tst.DebugLevel = Testing.DEBUG_CRITICAL

        tst.TestData("message", "Log Entry unit test")
        tst.Message("Testing Object Self Unit Test", Testing.DEBUG_INFO, "testing")

        tst.VerifyVariable("message", "Log Entry unit test")

        tst.Update()
        tst.Report()

class Row:
    def __init__(self):
        self._values = dict()

    def Set(self, strName, val):
        self._values[strName] = val

    def Get(self, strName):
        if strName not in self._values:
            return ''
        else:
            return self._values[strName]

    def ToXML(self):
        row_elem = ET.Element("row")
        for key, value in self._values.items():
            col_elem = ET.SubElement(row_elem, key)
            col_elem.text = str(value)
        return row_elem

class Table:
    def __init__(self):
        self._columns = []
        self._currentRow = Row()
        self._rows = [self._currentRow]

    def Set(self, colName, val):
        if colName not in self._columns:
            self._columns.append(colName)

        self._currentRow.Set(colName, val)

    def ToXML(self, name):
        tbl_elem = ET.Element("table", name=name)

        for rw in self._rows:
            tbl_elem.append(rw.ToXML())            

        return tbl_elem


class Database:    
    def __init__(self):
        self._tables = dict()

    def GetTable(self, szNme):
        if szNme not in self._tables:
            self._tables[szNme] = Table()

        return self._tables[szNme]

    def Export(self):
        root = ET.Element("database", version=str(Database.Version))

        for name, table in self._tables.items():
            root.append(table.ToXML(name))

        return root

    def pretty_xml(elem):
        rough = ET.tostring(elem, 'utf-8')
        reparsed = minidom.parseString(rough)
        return reparsed.toprettyxml(indent="    ")         


    
    
    
from sys import version
from _pytest.recwarn import T
import MoeSzyslakPython
from MoeSzyslakPython import *
from xml.dom import minidom

import time
import traceback
from typing import Type, List, Callable, Any
import datetime
import xml.etree.ElementTree as ET
import math
import UserClass

from UserClass import *

class VariableCollection:

    def __init__(self):
        self._array = []
        self._classIDs = []
        self._nObjects = 0
        self._objectMap = dict()

    def NewVariable(self, tag):
        if not isinstance(tag, str):
            raise TypeError("tag must be a string")

        var = VLVariable()         
        var.DisplayName = tag
        self.Add(var, tag, 0)
        return var

    def Add(self, obj, tag = '', nClassID = 0):
        if math.isnan(nClassID) or math.isinf(nClassID):
            raise ValueError("nClassID must be a finite number  (something went wrong with input most likely.)")

        if nClassID < 0:
            raise ValueError("nClassID cannot be negative")

        if not isinstance(tag, str):
            raise TypeError("tag must be a string")

        self._array.append(obj)
        self._classIDs.append(nClassID)
        self._nObjects += 1

        if (len(tag) > 0):
            self._objectMap[tag] = obj

    def Get(self, nme):
        return self._objectMap.get(nme)




## @class LogEntry
## @brief Represents a single log entry containing a timestamp and text message.
##
## This class stores a time value and an associated text string. It provides
## read‑only access to the timestamp through a property.
class LogEntry:

    def __init__(self, iunk):       
        self._iunk = iunk

    def __str__(self):
        return f"LogEntry(time={self._time!r}, text={self._text!r})"

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
    def text(self):
        return self._text
    
TESTING_IID = GUID("{7C6DA0F8-84AE-4E97-86F0-13BB1E67C713}")

class ITESTING(IUnknown):
    _iid_ = TESTING_IID
    _methods_ = [
         COMMETHOD(
            [], HRESULT, "Message",
            (['in'], c_wchar_p, "strMsg"),
            (['in'], c_int32, "nDebugLvl"),
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
            [], HRESULT, "RunTest",
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iPrp")
        ),

        COMMETHOD(
            [], HRESULT, "VerifyVariable",
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iPrp")
        ),

        COMMETHOD(
            [], HRESULT, "VerifyHResult",
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iPrp")
        ),

       
    ]

class Testing:

    DEBUG_FULL = 0
    DEBUG_VERBOSE = 1
    DEBUG_INFO = 2
    DEBUG_WARN = 3
    DEBUG_CRITICAL = 4

    def __init__(self, iTst):
        self._iTesting = iTst 
        print(f"m_iTesting = {self._iTesting}")
        self._logEntries = []

    def Message(self, strMsg, nDebugLvl):
        self._iTesting.Message(strMsg, nDebugLvl)
        

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

    def TestValue(self, name, val = None):        
        if val is not None:
            self._testValues[name] = val
            return val

        ret = self._testValues.get(name)

        if ret is None:
            ret = ''
        
        return ret

    def LogEntry(self, ndx):
        if 0 <= ndx < len(self._logEntries):
            return self._logEntries[ndx]
        else:
            raise IndexError("Log entry index out of range")

    def Report(self):
        print("Time\tMessage")
        for entry in self._logEntries:
            print(f"{entry.time:.2f}\t{entry.text}")

        if self._bPassed is True:
            print("Result:  PASS")
        else:
            print("Result:  FAILED")

    def Clear(self):
        self._testValues = {}
        self._bMemoryCheck = False
        self._bPassed = True
        self._logEntries = []
        self._testTime = datetime.datetime.now()

    def DebugLevel(self, dbgVal):
        self._iTesting.SetDebugLevel(dbgVal)
        print(f"Debug level set to {dbgVal}")

    def Update(self):
        unk_ptr = CPOINTER(IUnknown)()
        self._iTesting.GetLogEntry(0, byref(unk_ptr))
        self._logEntries.clear()
        nCnt = 0

        while unk_ptr:
            self._logEntries.append(LogEntry(unk_ptr))
            nCnt = nCnt + 1
            self._iTesting.GetLogEntry(nCnt, byref(unk_ptr))

    def UnitTest(cmpste):
        tst = cmpste.theTester()
        tst.DebugLevel(Testing.DEBUG_FULL)
        tst.Message("Testing Object Self Unit Test", Testing.DEBUG_INFO)
        tst.Update()

def test_self_test():

    if __version__  < 1573:
        raise ImportError(f"Incompatible version: testing.py is 1.3.6.{__version__ - 1530} but >= 1.3.6.{1590- 1573} is required.")
    tst = TestRunner()
    print("Running Self test 1")

    tst.TestValue("message", "test message 1")
    tst.RunTest(type(TestRunner))

    if tst.Passed is False:
        tst.Report()
    
    print("Running Self test 2")
    tst.TestValue("message", "")
    tst.RunTest(type(TestRunner))
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

    def UnitTest():
        tst = TestRunner()
        db = Database()

        try:
            tst.Message("Testing Database Unit Test")
            tst.Message(f"Database Version 1.3.6.{Database.Version - 1530}")
            tst.Message(f"Tested on {datetime.datetime.now()}")

            tbl = db.GetTable("test_values")
            tbl.Set("test_id", 1)
            tbl.Set("class_id", TestRunner.ClassID)

            tbl.Set("variable_name", "message")
            tbl.Set("variable_value", "test message 1")
            xml_str = Database.pretty_xml(db.Export())

            with open("test_database.xml", "w", encoding="utf-8") as f:
                f.write(xml_str)

        except Exception as e:
            tst.Verify(False, f"Exception during Database unit test: {e}")

        tst.Report()
        

if __name__ == "__main__":  
    sel = ""
    tst = TestRunner()

    while sel != "2":
        print("Class tester")
        print()
        print("1)  User")
        print("2)  Exit")
        sel = input("select:  ")

        if sel == "1":
            User.UnitTest(tst)
    
    
    
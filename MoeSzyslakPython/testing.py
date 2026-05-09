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

__version__ = 1573

class VLVariable:
    def __init__(self):
        self._displayName = ''
        self._bLocked = False
        self._str = ''

    def SetString(self, strVal):
        if not isinstance(strVal, str):
            raise TypeError("value must be a string")

        if self._bLocked is True:
            raise ValueError("Variable is locked and cannot be modified")

        self._str = strVal

    @property
    def DisplayName(self):
        return self._displayName

    @DisplayName.setter
    def DisplayName(self, new_value):
        if not isinstance(new_value, str):
            raise TypeError("DisplayName must be a string")

        self._displayName = new_value

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




## @class LogEntry
## @brief Represents a single log entry containing a timestamp and text message.
##
## This class stores a time value and an associated text string. It provides
## read‑only access to the timestamp through a property.
class LogEntry:
    Version = __version__
    ## @brief Constructs a new LogEntry.
    ##
    ## @param tme float representing ms since test start.
    ## @param txt string text message for this log entry.
    def __init__(self, tme: float, txt: str):
        if not isinstance(txt, str):
            raise TypeError("txt must be a string")

        if not isinstance(tme, float):
            raise TypeError("tme must be a float")

        if math.isnan(tme) or math.isinf(tme):
            raise ValueError("tme must be a finite number  (something went wrong with input most likely.)")

        if tme < 0:
            raise ValueError("tme cannot be negative")

        self._time = tme
        self._text = txt

        self._properties = VariableCollection()
        self._varTxt = self._properties.NewVariable("text")

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
    


class TestRunner:
    """
    A simple, extensible unit test runner that:
      - Lets the user choose which test class to run
      - Discovers test_ methods automatically
      - Times each test
      - Prints results and error messages
    """
    _timestart = time.perf_counter() * 1000
    ClassID = 398981
    
    def __init__(self):
        self.Clear()        

    def list_tests(self):
        """Print available test classes."""
        print("\nAvailable Test Suites:")
        for i, name in enumerate(self.test_classes.keys(), start=1):
            print(f"  {i}. {name}")

    def select_test_class(self) -> Type:
        """Prompt the user to choose a test class."""
        self.list_tests()
        choice = input("\nSelect a test suite by number: ")

        try:
            index = int(choice) - 1
            name = list(self.test_classes.keys())[index]
            return self.test_classes[name]
        except (ValueError, IndexError):
            print("Invalid selection.")
            return None

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

    def Message(self, strMsg):
        now = time.perf_counter() * 1000
        dur = now - self._timestart
        self._logEntries.append(LogEntry(dur, strMsg))

    def Verify(self, bVal, szMsg):
        if not bVal:
            self.Message(szMsg)
            self._bPassed = False

    def TestValue(self, name, val = None):        
        if val is not None:
            self._testValues[name] = val
            return val
        
        return self._testValues[name]

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

    def SelfTest(self):
        self.Message("Testing Object Self Unit Test")
        self.Message(f"Log Entry Version 1.3.6.{LogEntry.Version - 1530}")
        self._bPassed = True

        self._testTime = datetime.datetime.now()
        self.Message(f"Tested on {self._testTime}")

        self.Message(self._testValues['message'])  
        self.Verify(len(self._logEntries) > 2, "Log entry count < 2") 

    def Clear(self):
        self._testValues = {}
        self._bMemoryCheck = False
        self._bPassed = True
        self._logEntries = []
        self._testTime = datetime.datetime.now(); 


    @property
    def Passed(self):
        return self._bPassed

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
    Version = __version__

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
    test_self_test();
    Database.UnitTest()
    
    
    
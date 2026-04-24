from _pytest.recwarn import T
import MoeSzyslakPython
from MoeSzyslakPython import *

import time
import traceback
from typing import Type, List, Callable, Any

class LogEntry:
    def __init__(self, hTesting, ndx):
        self._hTesting = hTesting;
        self._ndx = ndx
        MoeSzyslakLibrary.Invoke(self._hTesting, "log {0} get text".format(self._ndx))
        self._strText = MoeSzyslakLibrary.GetReturnString(self._hTesting)

        MoeSzyslakLibrary.Invoke(self._hTesting, "log {0} get time".format(self._ndx))
        self._nTime = int(MoeSzyslakLibrary.GetReturnString(self._hTesting))


class TestRunner:
    """
    A simple, extensible unit test runner that:
      - Lets the user choose which test class to run
      - Discovers test_ methods automatically
      - Times each test
      - Prints results and error messages
    """
    _classID = 398981 

    def __init__(self):
        self.test_classes = {}
        MoeSzyslakLibrary.VerifyLibrary()
        self._hObj = MoeSzyslakLibrary.CreateHandle(TestRunner._classID)
        print('TestRunner._hObj = ' + str(self._hObj))

        self._testValues = {}
        self._bMemoryCheck = False
        self._bPassed = False
        self._logEntries = []

    def register(self, name: str, test_class: Type):
        """Register a test class under a user-friendly name."""
        self.test_classes[name] = test_class

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

    def RunTest(self, nClassID):
        """Main entry point to run selected tests."""
        if self._bMemoryCheck is True:
            MoeSzyslakLibrary.Invoke(self._hObj, 'set "Memory Check" TRUE')
        else:
            MoeSzyslakLibrary.Invoke(self._hObj, 'set "Memory Check" FALSE')

        print('RunTest {0}'.format(nClassID))
        MoeSzyslakLibrary.Invoke(self._hObj, 'RunTest {0}'.format(nClassID))
        print(MoeSzyslakLibrary.GetReturnString(self._hObj, 2000))

        MoeSzyslakLibrary.Invoke(self._hObj, "log get count")
        nCnt = int(MoeSzyslakLibrary.GetReturnString(self._hObj))
        print('Log Count:  {0}'.format(nCnt))
        self._logEntries.clear()

        for i in range(nCnt):
            self._logEntries.append(LogEntry(self._hObj, i))

        MoeSzyslakLibrary.Invoke(self._hObj, "get Passed")
        self._bPassed = MoeSzyslakLibrary.GetReturnString(self._hObj) == "TRUE";

        if self._bPassed is True:
            print(f"\033[92mResult:  PASS\033[0m")
        else:
            print("\033[91mResult:  FAILED\033[0m")

    def Message(self, strMsg):
        MoeSzyslakLibrary.Invoke(self._hObj, 'Message "{0}"'.format(strMsg))
        

    def TestValue(self, name, val = None):        
        if val is not None:
            MoeSzyslakLibrary.Invoke(self._hObj, 'SetTestData "{0}" "{1}"'.format(name, val)) 
            self._testValues[name] = val
            return val
        else:
            MoeSzyslakLibrary.Invoke(self._hObj, 'TestValues get "{0}"'.format(name))
            self._testValues[name] = MoeSzyslakLibrary.GetReturnString(self._hObj)

        return self._testValues[name]

    def UnitTest():
        tst = TestRunner()
        tst.TestValue("message", "Testing Object Self Unit Test")
        tst.Message("Testing Object Self Unit Test")
        tst.RunTest(TestRunner._classID)

        assert(tst.TestValue("message") == "Testing Object Self Unit Test")
        assert(len(tst._logEntries) == 5) 

    def test_run_test_method_success(self):
        """This allows the test class to test itself."""
        print("Running test_run_test_method_success...")
        

        



if __name__ == "__main__":
    tst = TestRunner()
    tst.select_test_class()
    
    
    
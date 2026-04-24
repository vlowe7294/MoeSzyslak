#pragma once
#include <chrono>
#include "Interfaces.h"
#include "VLString.h"
#include "Utilities/Database.h"

// There is one global instance of this class that is used to track memory usage during testing.  
// It can be used to detect memory leaks by comparing memory usage before and after tests are run.
// Each time an instance of a class is created that should be tracked, it calls IncrementInstance with its class ID.
// When an instance is destroyed, it calls DecrementInstance.  When the MemoryChecker is destroyed at program exit, 
// it reports any class IDs that still have instances allocated.

class MemoryChecker
{
public:
	MemoryChecker();
	~MemoryChecker();
	void IncrementInstance(UINT nClassID);
	void DecrementInstance(UINT nClassID);

private:
	int* m_pInstances;
	int* m_pclassIDs;
};

extern MemoryChecker g_memoryChecker;


/**
 * @class LogEntry
 * @brief Represents a single diagnostic or test log entry containing text,
 *        timestamp, and memory usage information.
 *
 * LogEntry implements the ILOGENTRY COM interface and provides access to
 * structured log data. Each entry stores a message, a timestamp, memory
 * usage at the time of logging, and a return string used by command
 * operations. Entries can be serialized to and from database tables.
 */
class LogEntry : public ILOGENTRY
{
public:
	/**
	 * @brief Constructs a new LogEntry.
	 * @param txt  The log message text.
	 * @param tme  Timestamp value (typically milliseconds since test start).
	 * @param mem  Memory usage value at the time of logging.
	 */
	LogEntry(wstring txt, long long tme, long long mem);

	/** @brief Destructor. */
	~LogEntry();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Retrieves the property collection associated with this entry.
	 * @param iProp Receives an IUnknown pointer to the property collection.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Properties(IUnknown** iProp);

	/**
	 * @brief Executes a command against this log entry.
	 *
	 * Supported commands:
	 * - `get <property>` — retrieves a property value.
	 *
	 * @param szCmd The command string.
	 * @return S_OK on success, or an HRESULT error code.
	 */
	HRESULT __stdcall Command(const wchar_t* szCmd);

	/**
	 * @brief Retrieves the return string produced by the last command.
	 * @param iStr Receives an IUnknown pointer to the return string.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall GetReturnString(IUnknown** iStr);

	HRESULT __stdcall UnitTest();

	/**
	 * @brief Writes this log entry to a database table.
	 * @param iTbl The table interface to write into.
	 */
	void Write(IUnknown* iTbl);

	/**
	 * @brief Reads this log entry from a database table.
	 * @param iTbl The table interface to read from.
	 */
	void Read(IUnknown* iTbl);

	wstring GetHTML();
	
private:
	int m_cRef;                         ///< COM reference count.
	VariableCollection* m_pProperties;  ///< Property collection for this entry.
	VLVariable* m_pTxt;                 ///< Log message text.
	VLVariable* m_pTime;                ///< Timestamp value.
	VLVariable* m_pMemUsed;             ///< Memory usage value.
	VLVariable* m_pReturnVar;           ///< Return string for command results.
};

/**
 * @class Testing
 * @brief Provides a complete automated testing framework for COM‑based
 *        components, including logging, verification, reporting, and
 *        subsystem‑specific test execution.
 *
 * Testing implements the ITESTING interface and supports:
 * - Running subsystem tests by class ID
 * - Logging messages with timestamps and memory usage
 * - Loading and saving test data
 * - Generating HTML and XML reports
 * - Verifying values and HRESULTs
 */	
class Testing : public TestingInf::ITESTING
{
public:
	/** @brief Constructs a new Testing object. */
	Testing();

	~Testing();

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Retrieves the property collection for this test runner.
	 * @param iPrp Receives an IUnknown pointer to the property collection.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Properties(IUnknown** iPrp);

	/**
	 * @brief Executes a command string against the testing framework.
	 *
	 * Supported commands include:
	 * - `get <property>`
	 * - `log <message>`
	 * - `load <file>`
	 * - `message <text>`
	 * - `runtest <classID>`
	 * - `set <name> <value>`
	 * - `settestdata <name> <value>`
	 * - `testvalues <command>`
	 * - `unittest`
	 *
	 * @param szCmd The command text.
	 * @return S_OK on success, or an HRESULT error code.
	 */
	HRESULT __stdcall Command(const wchar_t* szCmd);

	/**
	 * @brief Retrieves the return string from the last command.
	 * @param iStr Receives an IUnknown pointer to the return string.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall GetReturnString(IUnknown** iStr);

	/**
	 * @brief Runs a test suite based on a class ID.
	 * @param nClassID The class ID to test.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall RunTest(UINT nClassID);

	/**
	 * @brief Verifies that a stored test variable matches an expected value.
	 * @param varName The variable name.
	 * @param val     The expected value.
	 * @return S_OK if the values match, otherwise E_FAIL.
	 */
	HRESULT __stdcall VerifyVariable(LPCWSTR varName, LPCWSTR val);

	/**
	 * @brief Verifies that an HRESULT indicates success.
	 * @param hr    The HRESULT to check.
	 * @param szMsg The error message to log if the HRESULT fails.
	 * @return The original HRESULT.
	 */
	HRESULT __stdcall VerifyHResult(HRESULT hr, LPCWSTR szMsg);

	/**
	 * @brief Logs a message to the test report.
	 * @param szMsg The message text.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Message(LPCWSTR szMsg);

	/**
	 * @brief Loads test results and test values from a database file.
	 * @param szFilePath The path to the database file.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Load(LPCWSTR szFilePath);

	/**
	 * @brief Retrieves a test data value by name.
	 * @param szName The variable name.
	 * @param iStr   Receives an IUnknown pointer to the string interface containing the value.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall GetTestData(LPCWSTR szName, IUnknown* iStr);

	/**
	 * @brief Sets a test data value.
	 * @param szName The variable name.
	 * @param szVal  The value to assign.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal);
	

	HRESULT __stdcall HTMLPage(LPCWSTR szVar, IUnknown* iStr);
	HRESULT __stdcall UnitTest();

private:
	int m_cRef;                          ///< COM reference count.

	VLVariable* m_pPass;                 ///< Pass/fail indicator.
	VLVariable* m_pMemCheck;             ///< Memory usage check flag.
	VLVariable* m_pReportVar;            ///< Report output variable.
	VLVariable* m_pLogEntriesVar;        ///< Log entries list variable.
	VariableCollection* m_pProperties;   ///< Property collection.

	std::chrono::steady_clock::time_point m_timeStamp; ///< Test start time.
	MEMORYSTATUSEX m_memInfo;            ///< Memory usage snapshot.
	InterfaceCollectionInf m_iEntries;   ///< Collection of log entries.
	VLDateTime* m_pTestTime;             ///< Timestamp for test execution.
	StringInf m_iRetStr;                 ///< Return string buffer.
	VariableCollection* m_pTestValues;   ///< Loaded test data values.
	static const wchar_t* HTML_TEMPLATE;
	

	/** @brief Runs variable subsystem tests. */
	void VariableTest();

	/** @brief Runs TripPlanner subsystem tests. */
	void TripPlannerTest();

	/** @brief Runs Neverwinter subsystem tests. */
	void NeverwinterTest();

	/** @brief Runs iOS subsystem tests. */
	void IOSTest();

	/** @brief Runs user-defined tests. */
	void UserTest();

	/** @brief Runs finance subsystem tests. */
	void FinanceTest();

	/** @brief Runs a self-test of the Testing framework. */
	void SelfTest();

	/**
	 * @brief Internal verification helper.
	 * @param bVal  Boolean result of the test.
	 * @param szMsg Description of the test.
	 */
	void Verify(bool bVal, LPCWSTR szMsg);

	/**
	 * @brief Retrieves a test data value by name.
	 * @param szVarName The variable name.
	 * @return The value as a wstring.
	 */
	wstring GetTestData(LPCWSTR szVarName);

	/** @brief Generates the final report. */
	void Report();

};



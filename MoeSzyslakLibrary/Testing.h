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
 * @brief Represents a single structured diagnostic entry produced during testing.
 *
 * A LogEntry captures:
 *   - The log message text
 *   - The timestamp of when the entry was created
 *   - Memory usage at the time of logging
 *   - The debug level associated with the message
 *   - A category label for grouping or filtering
 *   - The delta time since the previous log entry
 *   - A unique, monotonically increasing entry ID
 *
 * LogEntry implements the ILOGENTRY COM interface and exposes its fields
 * through COM‑compatible getters (BSTR and primitive out‑parameters).
 *
 */

class LogEntry : public ILOGENTRY
{
public:
	/**
	 * @brief Constructs a new LogEntry with full diagnostic context.
	 *
	 * @param nDebugLvl  Debug/verbosity level associated with this entry.
	 * @param ctgry      Category or subsystem name for grouping/filtering.
	 * @param prevTme    Timestamp of the previous log entry, used to compute delta time.
	 *
	 * The constructor assigns a unique entry ID, initializes the property
	 * collection, computes delta time, and registers the allocation with
	 * the global MemoryChecker.
	 */
	LogEntry(wstring ctgry);
	~LogEntry();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;

	/**
	 * @brief Retrieves the log message text as a BSTR.
	 * @param bsTxt Receives the allocated BSTR.
	 * @return S_OK on success, E_OUTOFMEMORY on allocation failure.
	 */
	HRESULT __stdcall GetText(BSTR* bsTxt);
	HRESULT __stdcall SetText(const wchar_t* szTxt);

	/**
	 * @brief Retrieves the timestamp associated with this entry.
	 * @param tme Receives the timestamp value.
	 */
	HRESULT __stdcall GetTime(long long* tme);

	/**
	 * @brief Retrieves the memory usage value stored in this entry.
	 * @param mem Receives the memory usage value.
	 */
	HRESULT __stdcall GetMemUsed(long long* mem);

	inline HRESULT __stdcall SetDebugLevel(int nDebug)
	{
		m_nDebugLevel = nDebug;
		return S_OK;
	}

	inline HRESULT __stdcall GetDebugLevel(int* nDebug)
	{
		*nDebug = m_nDebugLevel;
		return S_OK;
	}

	HRESULT __stdcall UnitTest();

	/**
	 * @brief Writes this log entry to a database table.
	 * @param iTbl The table interface to write into.
	 */
	void Write(IUnknown* iTbl);

	/**
	 * @brief Loads this entry's property collection from a database table.
	 */
	void Read(IUnknown* iTbl);

	/**
     * @brief Generates an HTML table row representing this log entry.
     *
     * The HTML includes the message text and memory usage. Additional
     * fields (debug level, category, delta time) can be added as needed.
     *
     * @return A formatted HTML string.
     */
	wstring GetHTML();

	inline void SetFile(wstring fle) { m_file = fle; };
	inline void SetLine(int nLne) { m_line = nLne; };

	
	
private:
	int m_cRef;                         ///< COM reference count.
	int m_nID;                          ///< Unique entry ID.
	wstring m_txt;                      ///< Log message text.
	long long m_time;                   ///< Timestamp of the entry.
	long long m_memUsed;                ///< Memory usage at log time.
	UINT m_nDebugLevel;                 ///< Debug/verbosity level.
	wstring m_category;                 ///< Category or subsystem label.
	long long m_deltaTime;              ///< Time since previous log entry.
	wstring m_file;
	int m_line;
	static std::chrono::steady_clock::time_point m_timeStamp; ///< Test start time.
	static long long m_prevTime;
	DWORD m_threadID;
	DWORD m_processID;
};

struct TESTVALUE
{
	int nTestID;
	wchar_t name[50];
	wchar_t value[50];
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
	enum DEBUG_LEVEL
	{
		DEBUG_FULL,
		DEBUG_VERBOSE,
		DEBUG_INFO,
		DEBUG_WARN,
		DEBUG_CRITICAL,
		DEBUG_MAX
	};

	/** @brief Constructs a new Testing object. */
	Testing();

	~Testing();

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Logs a message to the test report.
	 * @param szMsg The message text.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Message(LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory);
	HRESULT __stdcall SetDebugLevel(int level);

	HRESULT __stdcall GetLogEntry(int ndx, IUnknown** iEntry);
	
	
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
	 * @brief Loads test results and test values from a database file.
	 * @param szFilePath The path to the database file.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall Load(LPCWSTR szFilePath);

	/**
	 * @brief Retrieves a test data value by name.
	 * @param szName The variable name.
	 * @param bsStr a BSTR containing the value.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall GetTestData(LPCWSTR szName, BSTR* bsStr);

	/**
	 * @brief Sets a test data value.
	 * @param szName The variable name.
	 * @param szVal  The value to assign.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal);

	HRESULT __stdcall GetClassName(UINT nClassID, IUnknown* iStrClassName);
	HRESULT __stdcall GetClassID(LPCWSTR szClassName, UINT* nClassID);
	
	/** @brief Generates the final report. */
	HRESULT __stdcall Report(wchar_t* szRpt, UINT nlen);

	/**
	 * @brief Internal verification helper.
	 * @param bVal  Boolean result of the test.
	 * @param szMsg Description of the test.
	 */
	HRESULT __stdcall Verify(BOOL bVal, LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory);

	inline HRESULT __stdcall GetPassed(BOOL* bVal) 
	{
		if (m_bPass)
			*bVal = TRUE;
		else
			*bVal = FALSE;
		
		return S_OK;
	}

	HRESULT __stdcall NewEntry(IUnknown** iEntry);

	HRESULT __stdcall UnitTest();

	inline void SetFile(wstring fle) { m_file = fle; };
	inline void SetLine(int nLne) { m_line = nLne; };
	LogEntry& GetLastEntry();	

private:
	int m_cRef;                          ///< COM reference count.
	wstring m_report;					 ///< Report output variable.
	bool m_bPass;		                 ///< Pass/fail indicator.

	VLVariable* m_pMemCheck;             ///< Memory usage check flag.
	
	VariableCollection* m_pProperties;   ///< Property collection.

	ComCollection m_entries;			 ///< Collection of log entries.
	VLDateTime* m_pTestTime;             ///< Timestamp for test execution.
	StringInf m_iRetStr;                 ///< Return string buffer.
	VariableCollection* m_pTestValues;   ///< Loaded test data values.
	static const int m_nTestValues;
	static const TESTVALUE m_testValues[2];
	int m_nTestNdx;
	DEBUG_LEVEL m_debugLevel;	
	wstring m_file;
	int m_line;

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
	 * @brief Retrieves a test data value by name.
	 * @param szVarName The variable name.
	 * @return The value as a wstring.
	 */
	wstring GetTestData(LPCWSTR szVarName);

	

};



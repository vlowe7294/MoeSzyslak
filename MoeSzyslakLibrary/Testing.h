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
*@class LogEntry
* @brief Represents a single structured diagnostic entry produced during testing.
*
* A LogEntry records all diagnostic context associated with a single testing event.
* Each entry captures :
*-The log message text
* -The timestamp at which the entry was created
* -The memory usage at the time of logging
* -The debug / verbosity level
* -A category label for grouping or filtering
* -The delta time since the previous log entry
* -A unique, monotonically increasing entry ID
* -The source file and line number where the entry originated
* -The thread ID and process ID of the caller
*
*LogEntry implements the ILOGENTRY COM interface and exposes its fields through
* COM‑compatible getters(BSTR and primitive out‑parameters).Empty strings are
* returned as NULL BSTRs, which are valid in COM and safely handled by C# callers.
*
* The class also maintains static timing state :
*-m_timeStamp : the test start time(steady_clock)
* -m_prevTime : the timestamp of the previous log entry
*
*These allow each entry to compute high‑resolution delta timing without requiring
* external coordination.
*/

class LogEntry : public ILOGENTRY
{
public:
	LogEntry(wstring ctgry);
	~LogEntry();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;

	/**
	 * @brief Retrieves the log message text as a BSTR.
	 *
	 * Empty strings produce a NULL BSTR, which is valid in COM and safely
	 * marshaled as null in C#. Non‑empty strings are allocated via SysAllocString.
	 *
	 * @param bsTxt Receives the allocated BSTR.
	 * @return S_OK on success, E_POINTER if bsTxt is null.
	 */
	HRESULT __stdcall GetText(BSTR* bsTxt);

	/**
	 * @brief Sets the log message text.
	 * @param szTxt Null‑terminated wide string containing the message text.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall SetText(const wchar_t* szTxt);

	
	/**
	 * @brief Retrieves the timestamp associated with this entry.
	 * @param tme Receives the timestamp value in milliseconds since test start.
	 */
	HRESULT __stdcall GetTime(long long* tme);

	/**
	 * @brief Retrieves the memory usage value stored in this entry.
	 * @param mem Receives the memory usage value.
	 */
	HRESULT __stdcall GetMemUsed(long long* mem);

	/**
	 * @brief Sets the debug/verbosity level for this entry.
	 */
	inline HRESULT __stdcall SetDebugLevel(int nDebug)
	{
		m_nDebugLevel = nDebug;
		return S_OK;
	}

	/**
	 * @brief Retrieves the debug/verbosity level.
	 */
	inline HRESULT __stdcall GetDebugLevel(int* nDebug)
	{
		*nDebug = m_nDebugLevel;
		return S_OK;
	}

	/**
	 * @brief Retrieves the category label as a BSTR.
	 */
	inline HRESULT __stdcall GetCategory(BSTR* bsCat)
	{
		*bsCat = SysAllocString(m_category.c_str());
		return S_OK;
	}

	/**
	 * @brief Sets the category label.
	 */
	inline HRESULT __stdcall SetCategory(const wchar_t* szCat)
	{
		m_category = szCat;
		return S_OK;
	}

	inline HRESULT __stdcall GetFile(BSTR* bsFile)
	{
		*bsFile = SysAllocString(m_file.c_str());
		return S_OK;
	}

	inline HRESULT __stdcall SetFile(const wchar_t* szFile)
	{
		m_file = szFile;
		return S_OK;
	}

	inline HRESULT __stdcall GetDeltaTime(long long* nDeltaTime)
	{
		*nDeltaTime = m_deltaTime;
		return S_OK;
	}

	inline HRESULT __stdcall GetLine(int* nLine)
	{
		*nLine = m_line;
		return S_OK;
	}

	inline HRESULT __stdcall SetLine(int nLine)
	{
		m_line = nLine;
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
	
private:
	int m_cRef;                         ///< COM reference count.
	int m_nID;                          ///< Unique entry ID.
	wstring m_txt;                      ///< Log message text.
	long long m_time;                   ///< Timestamp of the entry.
	long long m_memUsed;                ///< Memory usage at log time.
	UINT m_nDebugLevel;                 ///< Debug/verbosity level.
	wstring m_category;                 ///< Category or subsystem label.
	long long m_deltaTime;              ///< Time since previous log entry.
	wstring m_file;						///< Source file where the entry originated.
	int m_line;							///< Source line number.
	static std::chrono::steady_clock::time_point m_timeStamp; ///< Test start time.
	static std::atomic<long long> m_prevTime;				  ///< Timestamp of previous log entry.
	DWORD m_threadID;					///< Thread ID of caller.
	DWORD m_processID;					///< Process ID of caller.
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

	HRESULT __stdcall GetClassName(UINT nClassID, BSTR* bbsClassName);
	HRESULT __stdcall GetClassID(LPCWSTR szClassName, UINT* nClassID);
	
	/** @brief Generates the final report. */
	HRESULT __stdcall Report();

	/**
	 * @brief Internal verification helper.
	 * @param bVal  Boolean result of the test.
	 * @param szMsg Description of the test.
	 */
	HRESULT __stdcall Verify(BOOL bVal, LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory);
	HRESULT __stdcall NewEntry(IUnknown** iEntry);
	HRESULT __stdcall GetProperties(IUnknown** iPrp);

	HRESULT __stdcall UnitTest();

	inline void SetFile(wstring fle) { m_file = fle; };
	inline void SetLine(int nLne) { m_line = nLne; };
	LogEntry& GetLastEntry();	

private:
	int m_cRef;                          ///< COM reference count.
	VLVariable* m_pReport;				 ///< Report output variable.
	VLVariable* m_pPass;                 ///< Pass/fail indicator.
	VLVariable* m_pDebugLevel;
	VLVariable* m_pMemCheck;             ///< Memory usage check flag.
		
	VariableCollection* m_pProperties;   ///< Property collection.

	ComCollection m_entries;			 ///< Collection of log entries.	
	VariableCollection* m_pTestValues;   ///< Loaded test data values.
	int m_nTestNdx;
	
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



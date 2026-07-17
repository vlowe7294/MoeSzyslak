#include "pch.h"
#include <psapi.h>
#include "Testing.h"
#include "Utilities/Database.h"
#include "User.h"
#include "tinyxml2.h"

using namespace tinyxml2;

MemoryChecker g_memoryChecker;

MemoryChecker::MemoryChecker()
{
	m_pInstances = new int[100]();
	m_pclassIDs = new int[100]();
}

MemoryChecker::~MemoryChecker()
{
	int i = 0;

	while (i < 100 && m_pclassIDs[i] != 0)
	{
		if (m_pInstances[i] != 0)
			wprintf(L"Memory leak detected for class ID %d: %d instances still allocated.\n", m_pclassIDs[i], m_pInstances[i]);
		i++;
	}

	delete[] m_pInstances;
	delete[] m_pclassIDs;
}

void MemoryChecker::IncrementInstance(UINT nClassID)
{
	int i = 0;

	while (i < 100 && m_pclassIDs[i] != nClassID && m_pclassIDs[i] != 0)
		i++;

	if (i < 100)
	{
		m_pInstances[i]++;

		if (m_pclassIDs[i] == 0)
			m_pclassIDs[i] = nClassID;
	}
}

void MemoryChecker::DecrementInstance(UINT nClassID)
{
	int i = 0;

	while (i < 100 && m_pclassIDs[i] != nClassID && m_pclassIDs[i] != 0)
		i++;

	if (i < 100 && m_pclassIDs[i] == nClassID)
	{
		m_pInstances[i]--;
	}
}






UINT __stdcall GetLibraryVersion();
const wchar_t* GetLibraryPath();

std::chrono::steady_clock::time_point LogEntry::m_timeStamp = std::chrono::steady_clock::now();
std::atomic<long long> LogEntry::m_prevTime{ 0 };


// nDebugLvl - Verbosity/severity level for filtering.
// ctgry     - Subsystem/category label (e.g., "TripPlanner").
LogEntry::LogEntry(wstring ctgry)
{
	static int id = 0;
	id++;
	m_nID = id;
	
	m_cRef = 1;
	g_memoryChecker.IncrementInstance(LogEntryInf::ClassID);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_timeStamp);
	m_time = duration.count();

	static PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	m_memUsed = pmc.WorkingSetSize / 1024 / 1024; // MB

	m_nDebugLevel = 0;
	m_category = ctgry;
	m_deltaTime = m_time - m_prevTime.load();

	m_prevTime.store(m_time);
	m_line = 0;
	m_threadID = GetCurrentThreadId();
	m_processID = GetCurrentProcessId();
}

LogEntry::~LogEntry()
{
	printf("LogEntry destructor called\n");
	g_memoryChecker.DecrementInstance(LogEntryInf::ClassID);
}

HRESULT __stdcall LogEntry::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (!ppvObj)
		return E_POINTER;

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == LOGENTRY_IID)
	{
		*ppvObj = static_cast<ILOGENTRY*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall LogEntry::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall LogEntry::Release()
{
	m_cRef--;
	
	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

// COM STRING RETURN RULE:
// GetText allocates a BSTR using SysAllocString. The caller is responsible
// for freeing it with SysFreeString (C# and Python do this automatically).

HRESULT __stdcall LogEntry::GetText(BSTR* bsTxt)
{
	if (!bsTxt)
		return E_POINTER;

	// NULL is valid for empty strings
	*bsTxt = SysAllocString(m_txt.c_str());
	return S_OK;
}

HRESULT __stdcall LogEntry::SetText(const wchar_t* szTxt)
{
	m_txt = szTxt ? szTxt : L"";
	return S_OK;
}

HRESULT __stdcall LogEntry::GetTime(long long* tme)
{
	*tme = m_time;
	return S_OK;
}

HRESULT __stdcall LogEntry::GetMemUsed(long long* mem)
{
	*mem = m_memUsed;
	return S_OK;

}

HRESULT __stdcall LogEntry::UnitTest()
{
	SetText(L"Log Entry SetText()");
	return S_OK;
}

void LogEntry::Write(IUnknown* iTbl)
{
	
}

void LogEntry::Read(IUnknown* iTbl)
{
	
}

wstring LogEntry::GetHTML()
{
	wstring htm = L"\n      <tr>\n         <td></td>\n";
	htm += L"         <td>" + m_txt + L"</td>\n";
	htm += L"         <td>" + std::to_wstring(m_memUsed) + L"</td>\n      </tr>";
	return htm;
}

const int Testing::m_nTestValues = 2;
const TESTVALUE Testing::m_testValues[m_nTestValues] = 
{
	{ 0, L"message", L"Test Message Value 1" },
	{ 1, L"message", L"" }
};

Testing::Testing()
{
	m_cRef = 1;	
	m_pProperties = new VariableCollection();
	m_pReport = m_pProperties->NewVariable(L"Report");

	m_pPass = m_pProperties->NewVariable(L"Passed");
	m_pPass->SetAsBool(TRUE);

	m_pMemCheck = m_pProperties->NewVariable(L"Memory Check");
	m_pMemCheck->SetAsBool(FALSE);
	m_pTestTime = NULL;

	VLVariable* v = m_pProperties->NewVariable(L"Test Values");
	m_pTestValues = new VariableCollection();

	m_nTestNdx = 0;
	m_debugLevel = DEBUG_LEVEL::DEBUG_FULL;	
	m_line = 0;
}


Testing::~Testing()
{
	printf("Testing destructor called\n");
	m_pProperties->Release();

	if (m_pTestTime != NULL)
		m_pTestTime->Release();

	m_pTestValues->Release();
}

HRESULT __stdcall Testing::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == TestingInf::m_iid)
	{
		*ppvObj = static_cast<ITESTING*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Testing::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Testing::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Testing::VerifyVariable(LPCWSTR varName, LPCWSTR var)
{
	VLVariable* pVar = NULL;
	wstring s;

	if (!var || !varName)
		return E_INVALIDARG;

	pVar = m_pTestValues->GetByTag(varName);

	if (pVar != NULL)
	{
		s = pVar->GetString();
	}

	if (s != var)
	{
		wstring l = wstring(varName) + L" does not match " + var;
		Verify(false, l.c_str(), DEBUG_CRITICAL, L"Testing");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT __stdcall Testing::GetTestData(LPCWSTR szVarName, BSTR* bsStr)
{
	if (!bsStr)
		return E_POINTER;

	*bsStr = SysAllocString(GetTestData(szVarName).c_str());
	return S_OK;
}

HRESULT __stdcall Testing::GetClassID(LPCWSTR szClassName, UINT* nClassID)
{
	auto it = CLASS_NAMES.find(szClassName);
	*nClassID = 0;

	if (it != CLASS_NAMES.end())
		*nClassID = it->second;
	
	return S_OK;
}


HRESULT __stdcall Testing::SetTestData(LPCWSTR szVarName, LPCWSTR szVal)
{
	VLVariable* pVar = NULL;

	if (!szVarName || !szVal)
		return E_INVALIDARG;

	pVar = m_pTestValues->GetByTag(szVarName);

	if (pVar == NULL)
		pVar = m_pTestValues->NewVariable(szVarName);

	return pVar->SetString(szVal);

}

HRESULT __stdcall Testing::VerifyHResult(HRESULT hr, LPCWSTR szMsg)
{
	Verify(SUCCEEDED(hr), szMsg, 0, L"");
	return hr;
}

HRESULT __stdcall Testing::Message(LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory)
{
	LogEntry* l = NULL;

	if (nDebugLvl < 0 || nDebugLvl >= DEBUG_MAX)
		nDebugLvl = 0;

	NewEntry((IUnknown**)&l);
	l->SetText(szMsg);
	l->SetDebugLevel(nDebugLvl);
	l->Release();

	if (nDebugLvl >= m_debugLevel)
		wprintf(L"%s\n", szMsg);

	return S_OK;
}

HRESULT __stdcall Testing::SetDebugLevel(int nDebugLvl)
{
	if (nDebugLvl < 0 || nDebugLvl >= DEBUG_MAX)
	{
		wstring msg = std::to_wstring(nDebugLvl) + L" is an invalid debug level.";
		Verify(FALSE, msg.c_str(), DEBUG_CRITICAL, L"Testing");
		LogEntry& e = GetLastEntry();

		e.SetFile(L"Testing.cpp");
		e.SetLine(__LINE__);
		return E_FAIL;
	}

	m_debugLevel = (DEBUG_LEVEL)nDebugLvl;
	return S_OK;
}

HRESULT __stdcall Testing::GetLogEntry(int ndx, IUnknown** iEntry)
{
	*iEntry = m_entries.Get(ndx);
	return S_OK;
}

HRESULT __stdcall Testing::Load(LPCWSTR szFilePath)
{
	MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
	MoeInf<ITABLE, CLASSID::TABLE> iTbl;
	Database* pDB;
	Table* pTbl = NULL;
	LogEntry* iEnt = NULL;
	int i = 0;
	UINT nCnt = 0;

	iDB.Init();
	iDB->Load(szFilePath);	
	
	iDB->GetTable(L"testing_results", iTbl);
	pDB = (Database*)((IUnknown*)iDB);
	pTbl = (Table*)((IUnknown*)iTbl);

	m_entries.Clear();
	pTbl->GoToTopRow();
	pTbl->RowCount(&nCnt);

	while (i < nCnt)
	{
		iEnt = new LogEntry(L"");
		iEnt->Read(iTbl);
		pTbl->NextRow();

		m_entries.Add(iEnt, L"", LogEntryInf::ClassID);
		iEnt->Release();
		i++;
	}

	iDB->GetTable(L"test_values", iTbl);
	m_pTestValues->Load(iTbl);
	return S_OK;
}

HRESULT __stdcall Testing::GetClassName(UINT nClassID, BSTR* bbsClassName)
{
	
	wstring sClassName;

	for (auto& kv : CLASS_NAMES)
	{
		if (kv.second == nClassID)
			sClassName = kv.first.c_str();
	}

	*bbsClassName = SysAllocString(sClassName.c_str());
	return S_OK;
}

LogEntry& Testing::GetLastEntry()
{
	IUnknown* iunk = m_entries.Get(m_entries.Count() - 1);
	iunk->Release();
	return *(LogEntry*)iunk;

}

UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, const wchar_t* szCmd);
void _cdecl MoeSzyslakGetReturnString(UINT hObj, wchar_t* szRet, UINT len);

HRESULT __stdcall Testing::UnitTest()
{
	LogEntry* iEntry = NULL;
	NewEntry((IUnknown**)&iEntry);
	iEntry->UnitTest();
	iEntry->Release();

	VariableCollection* iPrp = NULL;

	GetProperties((IUnknown**)&iPrp);
	iPrp->Release();
	return S_OK;
}

HRESULT __stdcall Testing::Verify(BOOL bVal, LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory)
{
	IUnknown* iunk = NULL;

	if (bVal == FALSE)
	{
		Message(szMsg, nDebugLvl, szCategory);
		m_pPass->SetAsBool(FALSE);
	}

	return S_OK;
}

HRESULT __stdcall Testing::NewEntry(IUnknown** iEntry)
{
	LogEntry* l;
	

	l = new LogEntry(L"");
	m_entries.Add((IUnknown*)l, L"", LogEntryInf::ClassID);

	l->SetFile(m_file.c_str());
	l->SetLine(m_line);

	l->Release();
	l->QueryInterface(IID_IUnknown, (void**)iEntry);
	return S_OK;
}

HRESULT __stdcall Testing::GetProperties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	return S_OK;
}

void Testing::TripPlannerTest()
{
	TripPlannerInf iTrp;
	StringInf istr;
	IUnknown* iunk = NULL;
	
	try
	{
		Message(L"Trip Planner Unit Test", 0, L"");
		Message(L"Trip Name:\tChicago Trip", 0, L"");
		
		iTrp->Command(L"set \"Trip Name\" \"Chicago Trip\"");
		iTrp->Command(L"AddStop");
		iTrp->Command(L"AddStop");

		iTrp->GetReturnString(istr);
		wstring cnt = (const wchar_t*)istr;		

		VerifyHResult(iTrp->UnitTest(), L"Unit Test Failed.");

		iTrp->Command(L"get \"Trip Name\"");
		iTrp->GetReturnString(istr);
		Verify(wstring(istr) == L"Chicago Trip", L"Trip name should be Chicago Trip", 0, L"");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
		m_pPass->SetAsBool(FALSE);
	}
}

wstring Testing::GetTestData(LPCWSTR szVarName)
{
	VLVariable* pVar = NULL;

	pVar = m_pTestValues->GetByTag(szVarName);
	
	if (pVar != NULL)
	{
		return pVar->GetString();
	}
	else
		return L"";
}

HRESULT __stdcall Testing::Report()
{
	BOOL bVal = FALSE;
	long long tme = 0;
	LogEntryInf iEnt;
	BSTR bsTxt = NULL;
	wstring rept;
	
	m_pMemCheck->GetAsBool(&bVal);
	bool bMemCheck = bVal == TRUE;

	rept = L"Time\tMessage";

	if (bMemCheck)
		rept += L"\tMemory Available";

	while (m_entries.ForEach((IUnknown**)&iEnt))
	{
		iEnt->GetText(&bsTxt);
		iEnt->GetTime(&tme);
		rept += L"\n" + std::to_wstring(tme) + L"\t" + (const wchar_t*)bsTxt;
		SysFreeString(bsTxt);

		if (bMemCheck)
		{
			long long mem = 0;
			iEnt->GetMemUsed(&mem);
			rept += L"\t" + std::to_wstring(mem);
		}

		iEnt->Release();
	}

	rept += L"\n";
	m_pReport->SetString(rept.c_str());
	return S_OK;
}

UINT _cdecl CreateMoeSzyslakHandle(UINT nClassID);
void  _cdecl DestroyMoeSzyslakHandle(UINT hObj);


void Testing::VariableTest()
{
	VariableInf iVar;
	IUnknown* iunk = NULL;
	UINT nType = 0;
	StringInf iStr;
	int v = 0;
	double f = 0.0;
	wstring cmd;

	try
	{
		iVar.Init();
		Message(L"Variable Unit Test", 0, L"");
		
		cmd = L"set display \"" + m_pProperties->Get(L"name");
		cmd += L"\"";

		VerifyHResult(iVar->Command(cmd.c_str()), L"set failure");
		VerifyHResult(iVar->Command(L"set value true"), L"set failure");
		iVar->SetType(VLVariable::TYPE_BOOL);

		iVar->SetType(VLVariable::TYPE_STRING);

		cmd = L"set value \"" + m_pProperties->Get(L"value");
		cmd += L"\"";
		VerifyHResult(iVar->Command(cmd.c_str()), L"set value failure");
		VerifyHResult(iVar->Command(L"set display test_variable"), L"set display failure");

		VerifyHResult(iVar->SetLocked(TRUE), L"SetLocked failure");
		iVar->SetLocked(FALSE);

		iVar->SetType(VLVariable::TYPE_INT);
		
		iVar->Command(L"get display");
		iVar->GetReturnString(iStr);
		VerifyVariable(L"name", iStr);

		VerifyHResult(iVar->Command(L"get type"), L"get type failure");
		iVar->GetReturnString(iStr);
		nType = _wtoi(iStr);
		Verify(nType == VLVariable::TYPE_INT, L"type is not int", 0, L"");

		iVar->GetAsInt(2, &v);
		Verify(v == 29, L"int value is not 29", 0, L"");
		iVar->GetAsFloat(3, &f);

		Verify(f == 23.0, L"float value is not 23.0", 0, L"");
		iVar->Command(L"get value");
		iVar->GetReturnString(iStr);
		VerifyVariable(L"value", iStr);

		UINT hDB = CreateMoeSzyslakHandle(CLASSID::DATABASE);
		cmd = L"save ";
		cmd += std::to_wstring(hDB) + L" test_variables";
		VerifyHResult(iVar->Command(cmd.c_str()), L"save failure");

		DestroyMoeSzyslakHandle(hDB);

		VerifyHResult(iVar->UnitTest(), L"UnitTest returned failure");

		VerifyHResult(iVar->SetInterface(NULL, 0), L"SetInterface failure");

	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
		m_pPass->SetAsBool(FALSE);
	}
}

void Testing::NeverwinterTest()
{
	NeverwinterInf iNWN;
	VariableInfCollection iPrp;
	BOOL bPass = FALSE;
	MoeInf<IMODULE, CLASSID::MODULE> iMod;
	InterfaceCollectionInf iAreaList, iContents;
	MoeInf<IAREA, CLASSID::AREA> iArea;
	IUnknown* iunk = NULL;
	
	StringInf iStr;
		
	try
	{
		Message(L"Neverwinter Nights Unit Test", 0, L"");
		
		VerifyHResult(iNWN->Command(L"Module set \"Module Name\" \"Out of the Abyss\""), L"set module name failure");
		
		iNWN->GetReturnString(iStr);
		wstring s = (const wchar_t*)iStr;

		iNWN->Properties(iPrp);
		iPrp.GetVariableInterface(L"Module", iMod);

		iMod->Properties(iPrp);
		iPrp.GetVariableInterface(L"Areas", iAreaList);
		VerifyHResult(iAreaList->Get(0, iArea), L"Get Area failure");

		iPrp.GetVariableInterface(L"contents", iContents);
		iNWN->Command(L"Module area 0 set name \"Mooncrest Prison\"");

		VerifyHResult(iNWN->UnitTest(), L"UnitTest failure");

		iNWN->Command(L"Module get \"Module Name\"");
		iNWN->GetReturnString(iStr);
		Verify(wstring(iStr) == L"Out of the Abyss", L"Module name is not \"Out of the Abyss\"", 0, L"");

		//iNWN->Command(L"Module area 0 get name");
		//iNWN->GetReturnString(iStr);
		//Verify(wstring(iStr) == L"Mooncrest Prison", L"Area name is not Mooncrest Prison");

		//Verify(iNWN.Dispose(), L"NWN disposal failed  (elements were not freed properly"); 
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
		m_pPass->SetAsBool(FALSE);
	}
}

void Testing::IOSTest()
{
	IOSInf iIOS;
	CommsInf iComms;
	IUnknown* iunk = NULL;	

	try
	{
		Message(L"IOS Unit Test", 0, L"");
		iIOS.Attach(NULL);
		iIOS->Properties(&iunk);
		VariableInfCollection iPrp(iunk);

		iunk = NULL;
		iPrp.GetVariableInterface(L"Comm General Page", iComms);

		VerifyHResult(iIOS->Update(), L"Update failure");

		VerifyHResult(iIOS->UnitTest(), L"UnitTest failure");

		VerifyHResult(iIOS->Command(L"\"Comm General Page\" get \"Backup Audio Enabled\""), L"Command failed");
		
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
	}

}

void Testing::UserTest()
{
	UserInf iUser;
	MoeInf<IUSERSERVICE, CLASSID::USERSERVICE> iUserService;
	IUnknown* iunk = NULL;

	iUserService.Init();

	Message(L"User Unit Test", 0, L"");	
	VerifyHResult(iUser->UnitTest(), L"Unit test returned failure code");
}

void Testing::FinanceTest()
{
	InterfaceCollectionInf icoll;	
	int nCnt = 0;	
	FinanceInf iFnc;
	StringInf iStr;
	double fN = 0.0;
	IUnknown* iunk = NULL;
	
	try
	{
		iFnc.Attach();
		Message(L"Finance Unit Test", 0, L"");		

		VerifyHResult(iFnc->Command(L"account set name \"Wells Fargo\""), L"account set name failed");
		VerifyHResult(iFnc->Command(L"account set balance 1000.0"), L"account set balance failed");

		VerifyHResult(iFnc->Command(L"account AddTransaction"), L"account AddTransaction failed");

		VerifyHResult(iFnc->UnitTest(), L"Finance unit test failed");
		iFnc->Command(L"account get name");
		iFnc->GetReturnString(iStr);
		Verify((wstring)iStr == L"Wells Fargo", L"account name should be Wells Fargo.", 0, L"");

		iFnc->Command(L"account get balance");
		fN = _wtof(iStr);
		Verify(fN == 1000.0, L"account balance != 1000.0.", 0, L"");

		iFnc->Command(L"account transaction count");
		nCnt = _wtoi(iStr);
		Verify(nCnt == 1, L"# transactions != 1", 0, L"");
		
		icoll.Attach();	
		VerifyHResult(icoll->UnitTest(this), L"Interface Collection unit test failed");
		Verify(icoll.Dispose(), L"Dispose Failed.", 0, L"");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
	}

}

void Testing::SelfTest()
{
	TestingInf iTst;
	LogEntryInf iLog;
	UINT nID = 0;
	//StringInf iStr;
	BSTR iStr = NULL;
	IUnknown* iunk;
	
	try
	{
		iTst.Attach(this);
		
		VerifyHResult(iTst->GetClassID(L"trip planner", &nID), L"Get Class ID failed");
		Verify(nID == TripPlannerInf::ClassID, L"Class ID should be 507734", 0, L"");
		VerifyHResult(GetClassName(nID, &iStr), L"Get Class Name failed");

		Verify(wstring(iStr) == L"trip planner", L"Class name should be 'trip planner'", 0, L"");
		SysFreeString(iStr);

		VerifyHResult(iTst->UnitTest(), L"Testing unit test failed");
		iunk = m_entries.Get(4);
		iLog.Attach(iunk);

		iunk->Release();
		Verify((IUnknown*)iLog != NULL, L"iLog is NULL", 0, L"");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test", 0, L"");
	}

}





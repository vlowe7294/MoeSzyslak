#include "pch.h"
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

LogEntry::LogEntry(wstring txt, long long tme, long long mem, int nDebugLvl)
{
	static int id = 0;
	id++;
	m_nID = id;

	printf("LogEntry constructor called\n");

	m_cRef = 1;
	g_memoryChecker.IncrementInstance(LogEntryInf::ClassID);
	m_pProperties = new VariableCollection();
	
	m_pTime = m_pProperties->NewVariable(L"time");
	m_pTime->SetAsInt((int)tme, 0);
	m_pTime->SetLocked(true);

	m_pMemUsed = m_pProperties->NewVariable(L"memory used");
	m_pMemUsed->SetAsInt(mem, 0);
	m_pMemUsed->SetLocked(TRUE);

	m_pReturnVar = m_pProperties->NewVariable(L"return");
	m_nDebugLevel = nDebugLvl;
}

LogEntry::~LogEntry()
{
	printf("LogEntry destructor called\n");
	m_pProperties->Release();
	g_memoryChecker.DecrementInstance(LogEntryInf::ClassID);
}

HRESULT __stdcall LogEntry::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
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
	printf("LogEntry AddRef -> %lu\n", m_cRef);
	return m_cRef;
}

ULONG __stdcall LogEntry::Release()
{
	m_cRef--;
	printf("LogEntry Release -> %lu\n", m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall LogEntry::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
	return S_OK;
}

HRESULT __stdcall LogEntry::Command(const wchar_t* szCmd)
{
	HRESULT hr = E_FAIL;
	VLStringCollection cmds;

	if (!szCmd || wcslen(szCmd) == 0)
		return E_INVALIDARG;


	cmds.Split(szCmd, L' ');
	cmds.ToLower(0);
	m_pReturnVar->SetString(L"");

	switch (szCmd[0])
	{
	case L'g':
	case L'G':
		if (cmds.Compare(0, L"get"))
		{
			m_pReturnVar->SetString(m_pProperties->Get(cmds.Get(1)).c_str());
			hr = S_OK;
		}
		break;
	}

	return hr;
}

HRESULT __stdcall LogEntry::GetReturnString(IUnknown** iStr)
{	
	return m_pReturnVar->GetString(iStr);
}

void LogEntry::Write(IUnknown* iTbl)
{
	m_pProperties->Save(iTbl);
}

void LogEntry::Read(IUnknown* iTbl)
{
	m_pProperties->Load(iTbl);
}

wstring LogEntry::GetHTML()
{
	wstring htm = L"\n      <tr>\n         <td>" + m_pTime->GetString() + L"</td>\n";
	htm += L"         <td>" + m_txt + L"</td>\n";
	htm += L"         <td>" + m_pMemUsed->GetString() + L"</td>\n      </tr>";
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
	printf("Testing constructor called\n");
	
	m_pProperties = new VariableCollection();
	m_bPass = false;

	m_pMemCheck = m_pProperties->NewVariable(L"Memory Check");
	m_pMemCheck->SetAsBool(FALSE);

	m_iEntries.Attach();
	m_pLogEntriesVar = m_pProperties->NewVariable(L"Log Entries");
	m_pLogEntriesVar->SetInterface(m_iEntries, CLASSID::INTERFACELIST);

	m_timeStamp = std::chrono::high_resolution_clock::now();
	m_memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	m_pTestTime = NULL;

	VLVariable* v = m_pProperties->NewVariable(L"Test Values");
	m_pTestValues = new VariableCollection();
	v->SetInterface(m_pTestValues, CLASSID::INTERFACELIST);

	m_nTestNdx = 0;
	m_debugLevel = DEBUG_LEVEL::DEBUG_FULL;
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
	printf("AddRef -> %lu\n", m_cRef);
	return m_cRef;
}

ULONG __stdcall Testing::Release()
{
	m_cRef--;
	printf("Release -> %lu\n", m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Testing::RunTest(UINT nClassID)
{
	BOOL bPass = FALSE;
	wchar_t nme[MAX_COMPUTERNAME_LENGTH + 1];
	wstring s = L"Library path:  ";
	UINT v = GetLibraryVersion() - 1530;
	DWORD bufCharCount = MAX_COMPUTERNAME_LENGTH + 1;
	StringInf iStr;

	m_bPass = true;
	//m_iEntries->Clear();
	GetComputerName(nme, &bufCharCount);

	if (m_pTestTime != NULL)
		m_pTestTime->Release();

	m_pTestTime = new VLDateTime();

	s += GetLibraryPath();
	Message(s.c_str(), 0);
	iStr.Init();
	m_pTestTime->ToString(iStr);
	
	s = L"Tested on ";
	s += (const wchar_t*)iStr;
	Message(s.c_str(), 0);
	s = L"Version  1.3.6." + std::to_wstring(v);

	Message(s.c_str(), 0);
	s = L"Local Machine:\t" + wstring(nme);
	Message(s.c_str(), 0);

	switch (nClassID)
	{
	case VariableInf::ClassID:
		VariableTest();
		break;

	case IOSInf::ClassID:
		IOSTest();
		break;

	case UserInf::ClassID:
		UserTest();
		break;

	case FinanceInf::ClassID:
		FinanceTest();
		break;

	case TestingInf::ClassID:
		SelfTest();
		break;

	case TripPlannerInf::ClassID:
		TripPlannerTest();
		break;

	case NeverwinterInf::ClassID:
		NeverwinterTest();
		break;	

	default:
		Verify(false, L"Invalid class ID");
		break;
	}

	return S_OK;

}

HRESULT __stdcall Testing::VerifyVariable(LPCWSTR varName, LPCWSTR var)
{
	VLVariable* pVar = NULL;
	wstring s;

	if (!var || !varName)
		return E_INVALIDARG;

	m_pTestValues->GetByTag(varName, (IUnknown**)&pVar);

	if (pVar != NULL)
	{
		s = pVar->GetString();
	}

	if (s != var)
	{
		wstring l = wstring(varName) + L" does not match " + var;
		Verify(false, l.c_str());
		return E_FAIL;
	}

	return S_OK;
}

HRESULT __stdcall Testing::GetTestData(LPCWSTR szVarName, IUnknown* iunk)
{
	StringInf iStr;

	iStr.Attach(iunk);
	iStr.Set(GetTestData(szVarName).c_str());	
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

	m_pTestValues->GetByTag(szVarName, (IUnknown**)&pVar);

	if (pVar == NULL)
		pVar = m_pTestValues->NewVariable(szVarName);

	return pVar->SetString(szVal);

}

HRESULT __stdcall Testing::VerifyHResult(HRESULT hr, LPCWSTR szMsg)
{
	Verify(SUCCEEDED(hr), szMsg);
	return hr;
}

HRESULT __stdcall Testing::Message(LPCWSTR szMsg, int nDebugLvl)
{
	LogEntry* l;

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_timeStamp);

	long long ms = duration.count();

	GlobalMemoryStatusEx(&m_memInfo);
	DWORDLONG mem = m_memInfo.ullAvailPhys;
	mem = mem / 1024;
	mem = mem / 1024;

	if (nDebugLvl < 0 || nDebugLvl >= DEBUG_MAX)
		nDebugLvl = 0;

	l = new LogEntry(szMsg, ms, mem, nDebugLvl);
	m_iEntries->Add((IUnknown*)l, L"", LogEntryInf::ClassID);
	l->Release();

	if (nDebugLvl >= m_debugLevel)
		wprintf(L"%lld ms: %s\n", ms, szMsg);

	return S_OK;
}

HRESULT __stdcall Testing::SetDebugLevel(int nDebugLvl)
{
	if (nDebugLvl < 0 || nDebugLvl >= DEBUG_MAX)
	{
		wstring msg = std::to_wstring(nDebugLvl) + L" is an invalid debug level.";
		Verify(FALSE, msg.c_str());
		return E_FAIL;
	}

	m_debugLevel = (DEBUG_LEVEL)nDebugLvl;
	return S_OK;
}

HRESULT __stdcall Testing::GetLogEntry(int ndx, IUnknown** iEntry)
{
	m_iEntries->Get(ndx, iEntry);

	if (*iEntry != NULL)
		(*iEntry)->AddRef();

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

	m_iEntries->Clear();
	pTbl->GoToTopRow();
	pTbl->RowCount(&nCnt);

	while (i < nCnt)
	{
		iEnt = new LogEntry(L"", 0, 0, 0);
		iEnt->Read(iTbl);
		pTbl->NextRow();

		m_iEntries->Add(iEnt, L"", LogEntryInf::ClassID);
		iEnt->Release();
		i++;
	}

	iDB->GetTable(L"test_values", iTbl);
	m_pTestValues->Load(iTbl);
	return S_OK;
}

HRESULT __stdcall Testing::GetClassName(UINT nClassID, IUnknown* iunk)
{
	StringInf iStr;
	iStr.Attach(iunk);

	iStr.Set(L"");

	for (auto& kv : CLASS_NAMES)
	{
		if (kv.second == nClassID)
			iStr.Set(kv.first.c_str());
	}

	return S_OK;
}

UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, const wchar_t* szCmd);
void _cdecl MoeSzyslakGetReturnString(UINT hObj, wchar_t* szRet, UINT len);

HRESULT __stdcall Testing::UnitTest()
{
	IUnknown* iEntry = NULL;
	GetLogEntry(0, &iEntry);

	if (iEntry != NULL)
		iEntry->Release();

	return S_OK;
}

HRESULT __stdcall Testing::Verify(BOOL bVal, LPCWSTR szMsg)
{
	if (bVal == FALSE)
	{
		Message(szMsg, 0);
		m_bPass = false;
	}

	return S_OK;
}

void Testing::TripPlannerTest()
{
	TripPlannerInf iTrp;
	StringInf istr;
	IUnknown* iunk = NULL;
	
	try
	{
		Message(L"Trip Planner Unit Test", 0);
		Message(L"Trip Name:\tChicago Trip", 0);

		iTrp->Command(L"set \"Trip Name\" \"Chicago Trip\"");
		iTrp->Command(L"AddStop");
		iTrp->Command(L"AddStop");

		iTrp->GetReturnString(istr);
		wstring cnt = (const wchar_t*)istr;		

		VerifyHResult(iTrp->UnitTest(), L"Unit Test Failed.");

		iTrp->Command(L"get \"Trip Name\"");
		iTrp->GetReturnString(istr);
		Verify(wstring(istr) == L"Chicago Trip", L"Trip name should be Chicago Trip");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
	}
}

wstring Testing::GetTestData(LPCWSTR szVarName)
{
	VLVariable* pVar = NULL;

	m_pTestValues->GetByTag(szVarName, (IUnknown**)&pVar);
	
	if (pVar != NULL)
	{
		return pVar->GetString();
	}
	else
		return L"";
}

HRESULT __stdcall Testing::Report(wchar_t* szRpt, UINT nlen)
{
	BOOL bVal = FALSE;
	LogEntryInf iEnt;
	VariableInfCollection iPrp = NULL;	
	static int nCounter = 0;
	
	m_pMemCheck->GetAsBool(&bVal);
	bool bMemCheck = bVal == TRUE;

	m_report = L"Time\tMessage";

	if (bMemCheck)
		m_report += L"\tMemory Available";

	m_report += L"\n";

	while (m_iEntries->ForEach(iEnt) == S_OK)
	{
		nCounter++;
		iEnt->Properties(iPrp);
		m_report += iPrp.Get(L"time");
		m_report += L":  \t";

		m_report += iPrp.Get(L"text");

		if (bMemCheck == TRUE)
			m_report += L"\t" + iPrp.Get(L"memory used");

		m_report += L"\n";
	}

	wcsncpy_s(szRpt, nlen, m_report.c_str(), nlen - 5);
	m_iEntries->Clear();
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
		Message(L"Variable Unit Test", 0);

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
		Verify(nType == VLVariable::TYPE_INT, L"type is not int");

		iVar->GetAsInt(2, &v);
		Verify(v == 29, L"int value is not 29");		
		iVar->GetAsFloat(3, &f);

		Verify(f == 23.0, L"float value is not 23.0");
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
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
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
	
	StringInf iStr;
		
	try
	{
		Message(L"Neverwinter Nights Unit Test", 0);

		VerifyHResult(iNWN->Command(L"Module set \"Module Name\" \"Out of the Abyss\""), L"set module name failure");
		
		iNWN->GetReturnString(iStr);
		wstring s = (const wchar_t*)iStr;

		iNWN->Properties(iPrp);
		iPrp.GetVariableInterface(L"Module", iMod);

		iMod->Properties(iPrp);
		iPrp.GetVariableInterface(L"Areas", iAreaList);
		VerifyHResult(iAreaList->Get(0, iArea), L"Get Area failure");
		iArea->Properties(iPrp);

		iPrp.GetVariableInterface(L"contents", iContents);
		iNWN->Command(L"Module area 0 set name \"Mooncrest Prison\"");

		VerifyHResult(iNWN->UnitTest(), L"UnitTest failure");
		VerifyHResult(iArea->Tick(30), L"Area Tick() failure");

		for (int i = 0; i < 100; i++)
		{
			iArea->Tick(30);
		}

		iNWN->Command(L"Module get \"Module Name\"");
		iNWN->GetReturnString(iStr);
		Verify(wstring(iStr) == L"Out of the Abyss", L"Module name is not \"Out of the Abyss\"");

		//iNWN->Command(L"Module area 0 get name");
		//iNWN->GetReturnString(iStr);
		//Verify(wstring(iStr) == L"Mooncrest Prison", L"Area name is not Mooncrest Prison");

		//Verify(iNWN.Dispose(), L"NWN disposal failed  (elements were not freed properly"); 
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
	}
}

void Testing::IOSTest()
{
	IOSInf iIOS;
	CommsInf iComms;
	IUnknown* iunk = NULL;	

	try
	{
		Message(L"IOS Unit Test", 0);
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
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
	}

}

void Testing::UserTest()
{
	UserInf iUser;
	MoeInf<IUSERSERVICE, CLASSID::USERSERVICE> iUserService;

	iUserService.Init();
	

	Message(L"User Unit Test", 0);
	
	VerifyHResult(iUser->UnitTest(), L"Unit test returned failure code");
}

void Testing::FinanceTest()
{
	InterfaceCollectionInf icoll;	
	int nCnt = 0;	
	FinanceInf iFnc;
	StringInf iStr;
	double fN = 0.0;
	
	try
	{
		iFnc.Attach();
		Message(L"Finance Unit Test", 0);
		VerifyHResult(iFnc->Command(L"account set name \"Wells Fargo\""), L"account set name failed");
		VerifyHResult(iFnc->Command(L"account set balance 1000.0"), L"account set balance failed");

		VerifyHResult(iFnc->Command(L"account AddTransaction"), L"account AddTransaction failed");

		VerifyHResult(iFnc->UnitTest(), L"Finance unit test failed");
		iFnc->Command(L"account get name");
		iFnc->GetReturnString(iStr);
		Verify((wstring)iStr == L"Wells Fargo", L"account name should be Wells Fargo.");

		iFnc->Command(L"account get balance");
		fN = _wtof(iStr);
		Verify(fN == 1000.0, L"account balance != 1000.0.");

		iFnc->Command(L"account transaction count");
		nCnt = _wtoi(iStr);
		Verify(nCnt == 1, L"# transactions != 1");
		
		icoll.Attach();	
		VerifyHResult(icoll->UnitTest(this), L"Interface Collection unit test failed");
		Verify(icoll.Dispose(), L"Dispose Failed.");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
	}

}

void Testing::SelfTest()
{
	TestingInf iTst;
	LogEntryInf iLog;
	UINT nID = 0;
	StringInf iStr;
	
	try
	{
		iTst.Attach(this);
		iStr.Init();

		VerifyHResult(iTst->GetClassID(L"trip planner", &nID), L"Get Class ID failed");
		Verify(nID == TripPlannerInf::ClassID, L"Class ID should be 507734");
		VerifyHResult(GetClassName(nID, iStr), L"Get Class Name failed");

		Verify(wstring(iStr) == L"trip planner", L"Class name should be 'trip planner'");

		VerifyHResult(iTst->UnitTest(), L"Testing unit test failed");

		m_iEntries->Get(4, iLog);
		Verify((IUnknown*)iLog != NULL, L"iLog is NULL");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_bPass = false;
	}

}





#include "pch.h"
#include "Testing.h"
#include "Utilities/Database.h"
#include "User.h"

UINT __stdcall GetLibraryVersion();
const wchar_t* GetLibraryPath();

LogEntry::LogEntry(wstring txt, long long tme, long long mem)
{
	m_cRef = 1;
	m_pProperties = new VariableCollection();
	
	m_pTxt = m_pProperties->NewVariable(L"text");
	m_pTxt->SetString(txt.c_str());
	m_pTxt->SetLocked(true);

	m_pTime = m_pProperties->NewVariable(L"time");
	m_pTime->SetAsInt((int)tme, 0);
	m_pTime->SetLocked(true);

	m_pMemUsed = m_pProperties->NewVariable(L"memory used");
	m_pMemUsed->SetAsInt(mem, 0);
	m_pMemUsed->SetLocked(TRUE);

	m_pReturnVar = m_pProperties->NewVariable(L"return");
}

LogEntry::~LogEntry()
{
	m_pProperties->Release();
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

	cmds.Split(szCmd, L' ');
	m_pReturnVar->SetString(L"");

	switch (szCmd[0])
	{
	case L'g':
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
	m_pReturnVar->GetString(iStr);
	return S_OK;
}

void LogEntry::Write(IUnknown* iTbl)
{
	m_pProperties->Save(iTbl);
}

void LogEntry::Read(IUnknown* iTbl)
{
	m_pProperties->Load(iTbl);
}





Testing::Testing()
{
	m_cRef = 1;
	
	m_pProperties = new VariableCollection();
	m_pPass = m_pProperties->NewVariable(L"Passed");
	m_pPass->SetAsBool(FALSE);

	m_pMemCheck = m_pProperties->NewVariable(L"Memory Check");
	m_pMemCheck->SetAsBool(FALSE);
	m_pReportVar = m_pProperties->NewVariable(L"Report");

	m_iEntries.Attach();
	m_pLogEntriesVar = m_pProperties->NewVariable(L"Log Entries");
	m_pLogEntriesVar->SetInterface(m_iEntries, CLASSID::INTERFACELIST);

	m_timeStamp = std::chrono::high_resolution_clock::now();
	m_memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	m_pTestTime = NULL;

	VLVariable* v = m_pProperties->NewVariable(L"Test Values");
	m_pTestValues = new VariableCollection();
	v->SetInterface(m_pTestValues, CLASSID::INTERFACELIST);
}


Testing::~Testing()
{
	delete m_pProperties;

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

HRESULT __stdcall Testing::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
	return S_OK;
}

HRESULT __stdcall Testing::Command(const wchar_t* szCmd)
{
	HRESULT hr = E_FAIL;
	VLStringCollection cmds;
	
	cmds.Split(szCmd, L' ');
	cmds.ToLower(0);

	switch (szCmd[0])
	{
		case L'g':
			if (cmds.Compare(0, L"get"))
			{
				m_pProperties->Command(szCmd);
				m_pProperties->GetReturnString(m_iRetStr);
				m_pReportVar->SetString(m_iRetStr);
				hr = S_OK;
			}
			break;

		case L'l':
		case L'L':
			if (cmds.Compare(0, L"log"))
			{
				hr = m_iEntries->Command(cmds.SubString(1, L' ').c_str());
				m_iEntries->GetReturnString(m_iRetStr);
			}
			else if (cmds.Compare(0, L"load"))
			{
				hr = Load(cmds.Get(1).c_str());
			}
			break;

		case L'm':
		case L'M':
			if (cmds.Compare(0, L"message"))
			{
				hr = Message(cmds.Get(1).c_str());
			}
			break;

		case L'r':
		case L'R':
			if (cmds.Compare(0, L"runtest"))
			{
				hr = RunTest(_wtoi(cmds.Get(1).c_str()));
				m_pReportVar->GetString(m_iRetStr);
			}
			break;

		case L'S':
		case L's':
			if (cmds.Compare(0, L"settestdata"))
			{
				hr = SetTestData(cmds.Get(1).c_str(), cmds.Get(2).c_str());
				hr = S_OK;
			}
			else if (cmds.Compare(0, L"set"))
			{
				m_pProperties->Set(cmds.Get(1), cmds.Get(2), VLVariable::TYPE_STRING);
				hr = S_OK;
			}
			break;

		case L'u':
		case L'U':
			if (cmds.Compare(0, L"UnitTest"))
			{
				hr = UnitTest();
			}
			break;

	}

	return hr;
}

HRESULT __stdcall Testing::GetReturnString(IUnknown** iStr)
{
	*iStr = (IUnknown*)m_iRetStr;
	return S_OK;
}

HRESULT __stdcall Testing::RunTest(UINT nClassID)
{
	BOOL bPass = FALSE;
	wchar_t nme[MAX_COMPUTERNAME_LENGTH + 1];
	wstring s = L"Library path:  ";
	UINT v = GetLibraryVersion() - 1275;
	DWORD bufCharCount = MAX_COMPUTERNAME_LENGTH + 1;
	StringInf iStr;
	IOSInf iIOS;

	m_pPass->SetAsBool(TRUE);
	m_iEntries->Clear();
	GetComputerName(nme, &bufCharCount);

	if (m_pTestTime != NULL)
		m_pTestTime->Release();

	m_pTestTime = new VLDateTime();

	s += GetLibraryPath();
	Message(s.c_str());
	iStr.Init();
	m_pTestTime->ToString(iStr);

	s = L"Tested on ";
	s += (const wchar_t*)iStr;
	Message(s.c_str());
	s = L"Version  1.2.5." + std::to_wstring(v);

	Message(s.c_str());
	s = L"Local Machine:\t" + wstring(nme);
	Message(s.c_str());

	switch (nClassID)
	{
	case VariableInf::ClassID:
		VariableTest();
		break;

	case IOSInf::ClassID:
		IOSTest();
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

	Report();
	return S_OK;

}

HRESULT __stdcall Testing::VerifyVariable(LPCWSTR varName, LPCWSTR var)
{
	VLVariable* pVar = NULL;
	wstring s;

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
	return iStr->Set(GetTestData(szVarName).c_str());
}

HRESULT __stdcall Testing::SetTestData(LPCWSTR szVarName, LPCWSTR szVal)
{
	VLVariable* pVar = NULL;

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

HRESULT __stdcall Testing::Message(LPCWSTR szMsg)
{
	LogEntry* l;

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_timeStamp);

	long long ms = duration.count();

	GlobalMemoryStatusEx(&m_memInfo);
	DWORDLONG mem = m_memInfo.ullAvailPhys;
	mem = mem / 1024;
	mem = mem / 1024;

	l = new LogEntry(szMsg, ms, mem);
	m_iEntries->Add((IUnknown*)l, L"", CLASSID::LOGENTRY);
	l->Release();
	return S_OK;
}

HRESULT __stdcall Testing::Save(LPCWSTR szFilePath)
{
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

	iDB.Init();
	iDB->Load(szFilePath);	
	
	iDB->GetTable(L"testing_results", iTbl);
	pDB = (Database*)((IUnknown*)iDB);
	pTbl = (Table*)((IUnknown*)iTbl);

	m_iEntries->Clear();
	pTbl->GoToTopRow();

	while (i < pTbl->RowCount())
	{
		iEnt = new LogEntry(L"", 0, 0);
		iEnt->Read(iTbl);
		pTbl->NextRow();

		m_iEntries->Add(iEnt, L"", CLASSID::LOGENTRY);
		i++;
	}

	iDB->GetTable(L"test_values", iTbl);
	m_pTestValues->Load(iTbl);
	return S_OK;
}


UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, const wchar_t* szCmd);
void _cdecl MoeSzyslakGetReturnString(UINT hObj, wchar_t* szRet, UINT len);

HRESULT __stdcall Testing::UnitTest()
{
	
	Save(L"testing.db");	
	
	return S_OK;
}

void Testing::Verify(bool bVal, LPCWSTR szMsg)
{
	if (!bVal)
	{
		Message(szMsg);
		m_pPass->SetAsBool(FALSE);
	}
}

void Testing::TripPlannerTest()
{
	TripPlannerInf iTrp;
	IUnknown* iunk = NULL;

	try
	{
		Message(L"Trip Planner Unit Test");
		wstring d = L"Trip Name:\t" + GetTestData(L"Trip Name");
		Message(d.c_str());
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_pPass->SetAsBool(FALSE);
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

void Testing::Report()
{
	BOOL bVal = FALSE;
	wstring rep;
	LogEntry* iEnt = NULL;
	VariableInfCollection iPrp = NULL;		
	
	m_pMemCheck->GetAsBool(&bVal);
	bool bMemCheck = bVal == TRUE;

	rep = L"Time\tMessage";

	if (bMemCheck)
		rep += L"\tMemory Available";

	rep += L"\n";

	while (m_iEntries->ForEach((IUnknown**)&iEnt) == S_OK)
	{
		iEnt->Properties(iPrp);

		rep += iPrp.Get(L"time");
		rep += L":  \t";

		rep += iPrp.Get(L"text");

		if (bMemCheck == TRUE)
			rep += L"\t" + iPrp.Get(L"memory used");

		rep += L"\n";
	}

	m_pReportVar->SetString(rep.c_str());
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
		Message(L"Variable Unit Test");

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
	
	StringInf iStr;
		
	try
	{
		Message(L"Neverwinter Nights Unit Test");

		VerifyHResult(iNWN->Command(L"Module set \"Module Name\" Midnight"), L"set module name failure");
		
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
		VerifyHResult(iNWN->Command(L"EXit"), L"Exit Failure");
		VerifyHResult(iArea->Tick(30), L"Area Tick() failure");

		for (int i = 0; i < 100; i++)
		{
			iArea->Tick(30);
		}

		iNWN->Command(L"Module get \"Module Name\"");
		iNWN->GetReturnString(iStr);
		Verify(wstring(iStr) == L"Midnight", L"Module name is not Midnight");

		iNWN->Command(L"Module area 0 get name");
		iNWN->GetReturnString(iStr);
		Verify(wstring(iStr) == L"Mooncrest Prison", L"Area name is not Mooncrest Prison");

		Verify(iNWN.Dispose(), L"NWN disposal failed  (elements were not freed properly");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
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
		Message(L"IOS Unit Test");		
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
		m_pPass->SetAsBool(FALSE);
	}

}

void Testing::UserTest()
{
	UserInf iUser;
	IUnknown* iunk = NULL;
	wstring msg;

	m_pProperties->Set(L"Login Name", L"Vaughn", VLVariable::VAR_TYPE::TYPE_STRING);
	m_pProperties->Set(L"Password", L"ZFyZH8DuKemv", VLVariable::VAR_TYPE::TYPE_STRING);
	
	Message(L"User Unit Test");
	msg = L"Login name:  " + m_pProperties->Get(L"Login Name");
	Message(msg.c_str());

	iUser->Properties(&iunk);
	VariableInfCollection iPrp(iunk);

	iPrp.Set(L"Login Name", m_pProperties->Get(L"Login Name"));
	iPrp.Set(L"Password", m_pProperties->Get(L"Password"));

	VerifyHResult(iUser->UnitTest(), L"Unit test returned failure code");
	m_pProperties->Set(L"Is Logged In", iPrp.Get(L"Is Logged In"), VLVariable::VAR_TYPE::TYPE_BOOL);

	VerifyVariable(L"Login Name", iPrp.Get(L"Login Name").c_str());
	VerifyVariable(L"Is Logged In", L"TRUE");
	
	
}

void Testing::FinanceTest()
{
	InterfaceCollectionInf icoll;	
	int nCnt = 0;	

	try
	{
		icoll.Attach();	
		VerifyHResult(icoll->UnitTest(this), L"Interface Collection unit test failed");
		Verify(icoll.Dispose(), L"Dispose Failed.");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_pPass->SetAsBool(FALSE);
	}

}

void Testing::SelfTest()
{
	StringInf iStr;
	MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
	InterfaceCollectionInf iLog;
	TestingInf iTst;
	wstring cmd;
	IUnknown* iunk = NULL;
	MoeInf<ILOGENTRY, CLASSID::LOGENTRY> iEntry;
	VariableInfCollection iPrp;
	
	try
	{
		iTst.Attach(this);
		iStr.Init();
		
		VerifyHResult(iTst->GetTestData(L"message", iStr), L"GetTestData failed");

		cmd = L"Message \"" + wstring(iStr);
		cmd += L"\"";
		VerifyHResult(iTst->Command(cmd.c_str()), L"message failed");

		VerifyHResult(iTst->VerifyVariable(L"testing variable", L""), L"VerifyVariable failed");
		
		VerifyHResult(iTst->Command(L"log get count"), L"get log count failed");
		iTst->GetReturnString(iStr);
		Verify(_wtoi((const wchar_t*)iStr) == 5, L"Log count should be 5");

		iTst->Properties(&iunk);
		VariableInfCollection iPrp(iunk);
		iPrp.GetVariableInterface(L"Log Entries", iLog);

		iLog->Get(4, iEntry);		
		iEntry->Properties(iPrp);

		VerifyVariable(L"message", iPrp.Get(L"text").c_str());

		
		VerifyHResult(iTst->UnitTest(), L"Testing unit test failed");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_pPass->SetAsBool(FALSE);
	}

}
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

LogEntry::LogEntry(wstring txt, long long tme, long long mem)
{
	m_cRef = 1;
	g_memoryChecker.IncrementInstance(LogEntryInf::ClassID);
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

HRESULT __stdcall LogEntry::UnitTest()
{
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

wstring LogEntry::GetHTML()
{
	wstring htm = L"\n      <tr>\n         <td>" + m_pTime->GetString() + L"</td>\n";
	htm += L"         <td>" + m_pTxt->GetString() + L"</td>\n";
	htm += L"         <td>" + m_pMemUsed->GetString() + L"</td>\n      </tr>";
	return htm;
}


const wchar_t* Testing::HTML_TEMPLATE = LR"(
<!DOCTYPE html>
<html>
<head>
    <style>
        body {
            background-color: #1b1b2f;
            color: #eaeaea;
            font-family: "Trebuchet MS", serif;
            margin: 0;
            padding: 0;
        }
        .container {
            padding: 20px;
            height: 800px;
        }        
    </style>
    <title>Unit Testing</title>
</head> 
<body>
    <header>Unit Testing</header>
    <div>
        <form method="POST"></form>
    </div>
</body>
</html>
)";

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
	wstring s;

	if (!szCmd || wcslen(szCmd) == 0)
		return E_INVALIDARG;
	
	cmds.Split(szCmd, L' ');
	cmds.ToLower(0);

	switch (szCmd[0])
	{
		case L'g':
		case L'G':
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
			}
			else if (cmds.Compare(0, L"set"))
			{
				m_pProperties->Set(cmds.Get(1), cmds.Get(2), VLVariable::TYPE_STRING);
				hr = S_OK;
			}
			break;

		case L't':
		case L'T':
			if (cmds.Compare(0, L"testvalues"))
			{
				hr = m_pTestValues->Command(cmds.SubString(1, L' ').c_str());
				m_pTestValues->GetReturnString(m_iRetStr);
				m_pReportVar->SetString(m_iRetStr);
			}
			break;


		case L'u':
		case L'U':
			if (cmds.Compare(0, L"unittest"))
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
	UINT v = GetLibraryVersion() - 1530;
	DWORD bufCharCount = MAX_COMPUTERNAME_LENGTH + 1;
	StringInf iStr;

	m_pPass->SetAsBool(TRUE);
	//m_iEntries->Clear();
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
	s = L"Version  1.3.6." + std::to_wstring(v);

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

	Report();
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
	m_iEntries->Add((IUnknown*)l, L"", LogEntryInf::ClassID);
	l->Release();
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

		m_iEntries->Add(iEnt, L"", LogEntryInf::ClassID);
		iEnt->Release();
		i++;
	}

	iDB->GetTable(L"test_values", iTbl);
	m_pTestValues->Load(iTbl);
	return S_OK;
}

HRESULT __stdcall Testing::HTMLPage(LPCWSTR szVar, IUnknown* iunk)
{
	wstring htm = HTML_TEMPLATE, str;
	int nCnt = 0;
	LogEntry* iLog = NULL;
	StringInf iStr;

	iStr.Attach(iunk);

	int i = htm.find(L"</form");

	if (i > 0)
	{
		m_iEntries->Count(&nCnt);

		if (nCnt >= 0)
		{
			str = htm.substr(0, i);
			str += L"\n   <table>\n      <tr>\n         <th>Time</th>\n         <th>Result</th>\n         <th>Memory Used</th>\n      </tr>";
			while (m_iEntries.ForEach((IUnknown**)&iLog))
			{
				str += iLog->GetHTML();
			}


			str += L"\n</table>\n";


		}
		str += htm.substr(i);
		htm = str;
	}

	iStr.Set(htm.c_str());
	return S_OK;
}


UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, const wchar_t* szCmd);
void _cdecl MoeSzyslakGetReturnString(UINT hObj, wchar_t* szRet, UINT len);

HRESULT __stdcall Testing::UnitTest()
{
	StringInf iStr;

	iStr.Init();
	HTMLPage(L"", iStr);

	iStr.BufferSize(2000);
	wstring htm = (const wchar_t*)iStr;

	for (auto& kv : CLASS_NAMES)
		wprintf(L"%s\t%u\n", kv.first.c_str(), kv.second);

	auto it = CLASS_NAMES.find(L"Testing");
	UINT nclassID = 0;

	if (it != CLASS_NAMES.end())
		nclassID = it->second;

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
	StringInf istr;
	IUnknown* iunk = NULL;
	
	try
	{
		Message(L"Trip Planner Unit Test");
		Message(L"Trip Name:\tChicago Trip");

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
	LogEntryInf iEnt;
	VariableInfCollection iPrp = NULL;	
	static int nCounter = 0;
	
	m_pMemCheck->GetAsBool(&bVal);
	bool bMemCheck = bVal == TRUE;

	rep = L"Time\tMessage";

	if (bMemCheck)
		rep += L"\tMemory Available";

	rep += L"\n";

	while (m_iEntries->ForEach(iEnt) == S_OK)
	{
		nCounter++;
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
	StringInf iStr;
	wstring msg;
	VariableInfCollection iPrp;

	m_pTestValues->Set(L"Login Name", L"Vaughn", VLVariable::VAR_TYPE::TYPE_STRING);
	m_pTestValues->Set(L"Password", L"ZFyZH8DuKemv", VLVariable::VAR_TYPE::TYPE_STRING);
	
	Message(L"User Unit Test");
	VerifyHResult(iUser->Command(L"set \"Login Name\" Vaughn"), L"set login name failed");
	VerifyHResult(iUser->Command(L"set Password ZFyZH8DuKemv"), L"set password failed");
	
	iUser->Properties(iPrp);
	VerifyHResult(iUser->Command(L"Login"), L"Login failed");	
	VerifyHResult(iUser->UnitTest(), L"Unit test returned failure code");

	VerifyHResult(iUser->AddToMasterList(L"Nevin", L"password"), L"AddToMasterList failed");
	
	iUser->Command(L"get \"Login Name\"");
	iUser->GetReturnString(iStr);
	wprintf(L"Login name:  %s\n", (const wchar_t*)iStr);
	VerifyVariable(L"Login Name", (const wchar_t*)iStr);

	iUser->Command(L"get Password");
	wprintf(L"Password:  %s\n", (const wchar_t*)iStr);
	VerifyVariable(L"Password", (const wchar_t*)iStr);

	iUser->Command(L"get \"Is Logged In\"");  // this should be false as the unit test should artifically time out the login
	Verify(wstring((const wchar_t*)iStr) == L"FALSE", L"Is Logged In == TRUE");
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
		Message(L"Finance Unit Test");
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
		m_pPass->SetAsBool(FALSE);
	}

}

void Testing::SelfTest()
{
	TestingInf iTst;
	LogEntryInf iLog;
	
	try
	{
		iTst.Attach(this);

		VerifyHResult(iTst->UnitTest(), L"Testing unit test failed");

		m_iEntries->Get(4, iLog);
		Verify((IUnknown*)iLog != NULL, L"iLog is NULL");

		if ((IUnknown*)iLog != NULL)
			VerifyHResult(iLog->UnitTest(), L"LogEntry unit test failed");
	}
	catch (...)
	{
		Verify(false, L"Unhandled exception during test");
		m_pPass->SetAsBool(FALSE);
	}

}


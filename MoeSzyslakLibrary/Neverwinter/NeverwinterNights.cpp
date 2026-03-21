#include "pch.h"
#include "NeverwinterNights.h"

NeverwinterInf g_iNWN;

HRESULT nwn_module(const wchar_t* szParam)
{
	HRESULT hr = E_FAIL;
	IUnknown* iunk = NULL, *iRetStr = NULL;
	MoeInf<IMODULE, CLASSID::MODULE> iMod;
	
	if (g_iNWN == NULL)
		return hr;	

	g_iNWN->Properties(&iunk);
	VariableInfCollection iPrp(iunk);
	g_iNWN->GetReturnString(&iRetStr);

	iPrp.GetVariableInterface(L"Module", iMod);
	hr = iMod->Command(szParam, iRetStr);
}

HRESULT Exit(const wchar_t* szParam)
{
	if (g_iNWN == NULL)
		return E_FAIL;

	return g_iNWN->NWNExit();

}

HRESULT nwn_user(const wchar_t* szParam)
{
	HRESULT hr = S_OK;
	IUnknown* iunk = NULL;
	UserInf iUser;
	StringInf iNWNStr, iUserStr;

	if (g_iNWN == NULL)
		return E_FAIL;

	g_iNWN->Properties(&iunk);
	VariableInfCollection iPrp(iunk);

	iPrp.GetVariableInterface(L"User", iUser);
	hr = iUser->Command(szParam);
	iUser->GetReturnString(iUserStr);

	g_iNWN->GetReturnString(iNWNStr);
	iNWNStr->Append((const wchar_t*)iUserStr);
	return hr;
}

NeverwinterNights::NeverwinterNights()
{
	m_cRef = 1;

	m_iModule = new Module();
	VLVariable* v = m_properties.NewVariable(L"Module");
	v->SetInterface(m_iModule, 0);

	v = m_properties.NewVariable(L"User");
	v->SetInterface(m_iUser, UserInf::ClassID);

	m_funcList[L"exit"] = Exit;
	m_funcList[L"module"] = nwn_module;
	m_funcList[L"user"] = nwn_user;
	
	m_iReturnStr.Init();
	Load();
	Start();
}

NeverwinterNights::~NeverwinterNights()
{
	m_iModule->Release();
}

HRESULT __stdcall NeverwinterNights::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == NEVERWINTER_IID)
	{
		*ppvObj = static_cast<INEVERWINTER*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall NeverwinterNights::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall NeverwinterNights::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall NeverwinterNights::UnitTest()
{
	
	Area* pArea = NULL;
	VariableInfCollection iPrp;
	InterfaceCollectionInf iAreas, iContents;
	Creature* pCreature = NULL;
	LOCATION loc =
	{
		0.0,
		1000.0,
		0.0
	};

	m_iModule->Properties(iPrp);

	iPrp.GetVariableInterface(L"Areas", iAreas);

	iAreas->Get(0, (IUnknown**)&pArea);
	Area& ar = *pArea;	

	Placeable& plc = ar.GetPlaceable(0);
	plc.SetName(L"freshwater spring");

	ar.Properties(iPrp);
	iPrp.GetVariableInterface(L"contents", iContents);
	pCreature = new Creature();

	pCreature->SetName(L"Gert Addams");
	pCreature->SetLocation(loc);
	iContents->Add(pCreature, L"", CLASSID::CREATURE);

	pCreature->Release();	
	return S_OK;
}

HRESULT __stdcall NeverwinterNights::NWNExit()
{
	m_iReturnStr->Append(L"Shutting server down...\n");
	Save();
	return S_OK;
}

HRESULT __stdcall NeverwinterNights::Command(const wchar_t* szCmd)
{
	VLStringCollection wrds;
	HRESULT hr = S_OK;
	wstring prmpt;

	wrds.Split(szCmd, L' ');
	wrds.ToLower(0);
	g_iNWN.Attach(this);

	if (m_funcList.find(wrds.Get(0)) == m_funcList.end())
	{
		m_iReturnStr->Append(L"Unknown command - ");
		m_iReturnStr->Append(wrds.Get(0).c_str());
		m_iReturnStr->Append(L"\n");
		hr = E_FAIL;
	}	
	else
		hr = m_funcList[wrds.Get(0)](wrds.SubString(1, L' ').c_str());

	return hr;

}

HRESULT __stdcall NeverwinterNights::Dispose()
{
	return S_OK;
}

HRESULT __stdcall NeverwinterNights::GetReturnString(IUnknown** iStr)
{
	*iStr = (IUnknown*)m_iReturnStr;
	return S_OK;
}

HRESULT __stdcall NeverwinterNights::Properties(IUnknown** iProp)
{
	m_properties.QueryInterface(IID_IUnknown, (void**)iProp);
	m_properties.Release();
	return S_OK;
}

void NeverwinterNights::Start()
{
	wstring cmdline, prevCmd;
	StringInf istr;
	IUnknown* iunk = NULL;

	istr.Init();

	VersionAsString(istr);

	m_iModule->Properties(&iunk);
	VariableInfCollection iPrp(iunk);

	m_iReturnStr->Append(iPrp.Get(L"Module Name").c_str());
	m_iReturnStr->Append(L" ");
	m_iReturnStr->Append((const wchar_t*)istr);

	VLDateTime now_time;

	now_time.ToString(istr);

	m_iReturnStr->Append(L"\nServer start time ");
	m_iReturnStr->Append((const wchar_t*)istr);
	m_iReturnStr->Append(L"\n");
}

void NeverwinterNights::Load()
{
	MoeInf<IDATABASE, CLASSID::DATABASE> iDB;

	iDB.Init();
	Database* pDB = (Database*)((IUnknown*)iDB);

	if (SUCCEEDED(iDB->Load(L"new_nwn.db")))
		m_iModule->Load(*pDB);

}

void NeverwinterNights::Save()
{
	Database* pDB = new Database();

	m_iModule->Save(pDB);
	pDB->Save(L"new_nwn.db");

	pDB->Release();

}
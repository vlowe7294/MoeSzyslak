#include "..\pch.h"
#include "NeverwinterNights.h"

UINT Room::m_lastID;
Room::DIRECTION Room::OPPOSITE_DIRECTION[MAX_DIRECTIONS] =
{
	DIRECTION::SOUTH,
	DIRECTION::NORTH,
	DIRECTION::WEST,
	DIRECTION::EAST,
	DIRECTION::DOWN,
	DIRECTION::UP
};

Room::Room()
{
	m_id = m_lastID;
	m_lastID++;
	m_cRef = 1;
	memset(m_exits, 0, sizeof(m_exits));
}

Room::~Room()
{
	for (int i = 0; i < MAX_DIRECTIONS; i++)
	{
		if (m_exits[i] != NULL)
			m_exits[i]->Release();
	}
}

HRESULT __stdcall Room::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;
}

ULONG __stdcall Room::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Room::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

void Room::AddExit(DIRECTION dir)
{
	DIRECTION rev;

	Room* pExitRoom = new Room();

	if (m_exits[dir] != NULL)
		m_exits[dir]->Release();

	m_exits[dir] = pExitRoom;

	rev = OPPOSITE_DIRECTION[dir];
	pExitRoom->m_exits[rev] = this;
	AddRef();
}




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

	m_funcList[L"module"] = nwn_module;
	m_funcList[L"user"] = nwn_user;
	
	m_iReturnStr.Init();
	Load();
	Start();

	m_intro = 
		L"\n=== NOCTURNE LEGENDS ===\n"
		L"In the twilight lands of Nocturne, where moonlit forests whisper forgotten tales\n"
		L"and ancient ruins hum with sleeping magic, every wanderer carries the spark of a legend.\n"
		L"Here, fates are shaped not by prophecy, but by the choices of those who dare to roam.\n"
		L"Welcome, traveler, to a realm where stories awaken in the shadows.\n";

	m_pStartingRoom = new Room();
}

NeverwinterNights::~NeverwinterNights()
{
	m_iModule->Release();
	m_pStartingRoom->Release();
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
	
	m_iModule->Properties(iPrp);

	iPrp.GetVariableInterface(L"Areas", iAreas);

	iAreas->Get(0, (IUnknown**)&pArea);
	Area& ar = *pArea;	

	ar.Properties(iPrp);
	iPrp.GetVariableInterface(L"contents", iContents);
	pCreature = new Creature(L"Gert Addams");

	iContents->Add(pCreature, L"", CLASSID::CREATURE);

	pCreature->Release();

	ShowIntro();

	m_pStartingRoom->AddExit(Room::NORTH);

	return S_OK;
}

HRESULT __stdcall NeverwinterNights::Command(const wchar_t* szCmd)
{
	HRESULT hr = S_OK;

	VLStringCollection wrds;
	
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

HRESULT __stdcall NeverwinterNights::Save(const wchar_t* szFilePath)
{
	Database* pDB = new Database();

	m_iModule->Save(pDB);
	pDB->Save(szFilePath);

	pDB->Release();
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



void NeverwinterNights::ShowIntro()
{
	wprintf(L"%s", m_intro.c_str());	
}
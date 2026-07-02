#include "pch.h"
#include "Area.h"

Dice::Dice(UINT nSides, UINT nSeed) : m_rng(nSeed)
{
	m_nSides = nSides;
}

Dice::~Dice()
{

}

UINT Dice::Roll(UINT nRolls)
{
	UINT nRet = 0;

	std::uniform_int_distribution<int> dist(1, m_nSides);

	for (int i = 0; i < nRolls; i++)
	{
		nRet += dist(m_rng);
	}

	return nRet;

}


Encounter::Encounter(wstring tag, DIFFICULTY dif, int nMax)
{
	m_tag = tag;
	m_difficulty = dif;
	m_maximumCreatures = nMax;
}

Encounter::~Encounter()
{

}




Location::Location(const wchar_t* szName)
{
	m_name = szName;
	m_cRef = 1;
	m_pEncounter = NULL;

}

Location::~Location()
{
	printf("Location destructor called\n");
	delete m_pEncounter;
}

HRESULT __stdcall Location::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == LOCATION_IID)
	{
		*ppvObj = static_cast<ILOCATION*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;
}

ULONG __stdcall Location::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG __stdcall Location::Release()
{
	InterlockedDecrement(&m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Location::AddNeighbor(IUnknown* iunk, int travelTimeSec)
{
	Location* iLoc = NULL;
	printf("Location::AddNeighbor() line 100\n");

	if (iunk == NULL)
		return E_FAIL;

	iunk->QueryInterface(LOCATION_IID, (void**)&iLoc);

	if (iLoc == NULL)
		return E_FAIL;
	
	m_neighbors.push_back(iLoc);
	m_travelTimes.push_back(travelTimeSec);
	iLoc->Release();
	return S_OK;
}

HRESULT __stdcall Location::AddEncounter(const wchar_t* szTag, int dif, int nMax)
{
	printf("Location::AddEncounter() line 118\n");

	if (dif < 0 || dif >= Encounter::DIFFICULTY_MAX)
		return E_FAIL;

	delete m_pEncounter;
	m_pEncounter = new Encounter(szTag, (Encounter::DIFFICULTY)dif, nMax);
	return S_OK;
}

void Location::Tick(int nSec)
{

}




Area::Area(const wchar_t* szName, int nDanger)
{
	m_cRef = 1;
	g_memoryChecker.IncrementInstance(CLASSID::AREA);

	m_pProperties = new VariableCollection();

	m_name = szName;

	VLVariable* v = m_pProperties->NewVariable(L"contents");
	
	m_nDangerLevel = nDanger;
	m_pQuest = new Quest(L"Gather Herbs at the Forest Edge", 1, 0);
}

Area::~Area()
{
	g_memoryChecker.DecrementInstance(CLASSID::AREA);
	m_pProperties->Release();
	delete m_pQuest;
}

HRESULT __stdcall Area::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == AREA_IID)
	{
		*ppvObj = static_cast<IAREA*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Area::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Area::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Area::Command(const wchar_t* szCmd, IUnknown* iRetStr)
{
	VLStringCollection wrds;
	StringInf iStr;
	
	iStr.Attach(iRetStr);

	wrds.Split(szCmd, L' ');
	wrds.ToLower(0);

	if (wrds.Compare(0, L"set"))
	{
		m_pProperties->Set(wrds.Get(1), wrds.Get(2), VLVariable::VAR_TYPE::TYPE_STRING);
		return S_OK;
	}
	else if (wrds.Compare(0, L"get"))
	{
		iStr->Set(m_pProperties->Get(wrds.Get(1)).c_str());
		return S_OK;
	}

	iStr->Set(L"Unknown Command");
	return E_FAIL;

}

HRESULT __stdcall Area::Tick(int nSec)
{
	Location* pLoc = NULL;
		
	while (m_locations.ForEach((IUnknown**)&pLoc) == S_OK)
	{
		if (pLoc != NULL)
		{
			pLoc->Tick(nSec);
		}			
	}

	return S_OK;
}

HRESULT __stdcall Area::AddLocation(IUnknown** iLoc, const wchar_t* locName)
{
	Location* pLoc = new Location(locName);
	m_locations.Add(pLoc, locName, 0);
	pLoc->QueryInterface(IID_IUnknown, (void**)iLoc);
	pLoc->Release();
	return S_OK;
}


void Area::Save(Table& tbl)
{
	tbl.Set(L"area_name", m_name.c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
	tbl.Set(L"danger_level", std::to_wstring(m_nDangerLevel).c_str(), VLVariable::VAR_TYPE::TYPE_INT);
}

void Area::Load(Table& tbl)
{
	m_pProperties->Load(&tbl);
}

Location& Area::GetLocation(int ndx)
{
	Location* iLoc = NULL;
	m_locations.Get(ndx, (IUnknown**)&iLoc);
	return *iLoc;
}

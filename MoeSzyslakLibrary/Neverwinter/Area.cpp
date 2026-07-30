#include "pch.h"
#include "Area.h"
#include "..\AstralWorkshop\Character.h"

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




Location::Location()
{
	m_pProperties = new VariableCollection();
	m_pName = m_pProperties->NewVariable(L"Name");

	m_cRef = 1;
	m_pEncounter = NULL;
	m_pPlaceable = NULL;

}

Location::~Location()
{
	printf("Location destructor called\n");
	delete m_pEncounter;

	if (m_pPlaceable != NULL)		
		m_pPlaceable->Release();

	m_pProperties->Release();
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

HRESULT __stdcall Location::AddPlaceable(const wchar_t* szName, const wchar_t* szTag)
{
	if (m_pPlaceable == NULL)
		m_pPlaceable = new Placeable();

	m_pPlaceable->SetName(szName);
	m_pPlaceable->SetTag(szTag);
	return S_OK;
}

HRESULT __stdcall  Location::GetProperties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	return S_OK;
}

void Location::Tick(int nSec)
{
	if (m_pPlaceable != NULL)
		m_pPlaceable->Tick(nSec);
}

void Location::GetObjectByTag(IUnknown** iunk, const wchar_t* szTag)
{
	if (m_pPlaceable != NULL && m_pPlaceable->GetTag() == szTag)
	{
		m_pPlaceable->QueryInterface(IID_IUnknown, (void**)iunk);
		return;
	}		

}

// event functions

void MakeEvil(Character& pc)
{
	pc.MessageString(L"Just being in this place is enough to taint your soul.");
	pc.AdjustAlignment(Creature::ALIGNMENT_EVIL, 1);	
}



void rav_applymists(IUnknown* iSelf, IUnknown* iEntering)
{
	ICREATURE* iPC = NULL;
	BOOL bIsPC = FALSE;
	Dice d(2), d15(15);
	
	iEntering->QueryInterface(CREATURE_IID, (void**)&iPC);
	

	if (iPC == NULL)
		return;

	iPC->GetIsPC(&bIsPC);

	if (bIsPC == FALSE)
	{
		iPC->Release();
		return;
	}

	Character* pPC = (Character*)iPC;

	MakeEvil(*pPC);	
	iPC->Release();
}




Area::Area()
{
	m_cRef = 1;
	g_memoryChecker.IncrementInstance(CLASSID::AREA);

	m_pProperties = new VariableCollection();
	m_pName = m_pProperties->NewVariable(L"Name");
	m_pDangerLevel = m_pProperties->NewVariable(L"Danger Level");

	m_pDangerLevel->SetAsInt(0, 0);
	m_pDangerLevel->SetLimits(0, 20);
	Quest* pQuest = new Quest(L"Gather Herbs at the Forest Edge", 1, 0);
	m_quests.Add(pQuest, L"", 0);
	pQuest->Release();
}

Area::~Area()
{
	g_memoryChecker.DecrementInstance(CLASSID::AREA);
	m_pProperties->Release();
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

HRESULT __stdcall Area::AddLocation(IUnknown** iLoc)
{
	Location* pLoc = new Location();
	m_locations.Add(pLoc, L"", 0);
	pLoc->QueryInterface(IID_IUnknown, (void**)iLoc);
	pLoc->Release();
	return S_OK;
}

HRESULT __stdcall Area::GetProperties(IUnknown** iVarList)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iVarList);
	return S_OK;
}

HRESULT __stdcall Area::AddQuest(IUnknown** iQuest, const wchar_t* szName, int nDuration, int nDifficulty)
{
	Quest* pQuest = new Quest(szName, nDuration, nDifficulty);
	pQuest->QueryInterface(IID_IUnknown, (void**)iQuest);
	m_quests.Add(pQuest, szName, 0);
	pQuest->Release();
	return S_OK;
}

HRESULT __stdcall Area::GetLocation(int ndx, IUnknown** iLoc)
{
	return m_locations.Get(ndx, iLoc);
}

HRESULT __stdcall Area::UnitTest()
{
	Character* pChar = new Character(L"Alyntha Addams", Creature::CLASS_ROGUE, Character::BACKGROUND_PEASANT);
	IUnknown* iSpawn = NULL;

	OnEnter(pChar);
	pChar->Release();
	GetObjectByTag(&iSpawn, L"AOEcentre");

	if (iSpawn != NULL)
		iSpawn->Release();

	return S_OK;
}

HRESULT __stdcall Area::SetOnEnterHandler(const wchar_t* szFnName)
{
	if (wstring(szFnName) == L"rav_applymists")
		m_onEnterHandler = rav_applymists;
	else
		m_onEnterHandler = NULL;

	return S_OK;
}

void Area::Save(Table& tbl)
{
	int nDnger = 0;
	tbl.Set(L"area_name", m_pName->GetString().c_str(), VLVariable::VAR_TYPE::TYPE_STRING);

	m_pDangerLevel->GetAsInt(0, &nDnger);
	tbl.Set(L"danger_level", std::to_wstring(nDnger).c_str(), VLVariable::VAR_TYPE::TYPE_INT);
}

void Area::Load(Table& tbl)
{
	m_pProperties->Load(&tbl);
}



void Area::OnEnter(IUnknown* iEnteringObj)
{
	if (m_onEnterHandler)
		m_onEnterHandler(this, iEnteringObj);
	
}

void Area::GetObjectByTag(IUnknown** iunk, const wchar_t* szTag)
{
	Location* iLoc = NULL;

	while (m_locations.ForEach((IUnknown**)&iLoc) == S_OK)
	{
		if (*iunk == NULL)
			iLoc->GetObjectByTag(iunk, szTag);
		
	}

}

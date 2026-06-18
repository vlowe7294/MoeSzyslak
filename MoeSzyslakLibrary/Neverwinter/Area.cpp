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




Location::Location(const wchar_t* szName)
{
	m_name = szName;
	m_cRef = 1;

}

Location::~Location()
{

}

HRESULT __stdcall Location::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

void Location::AddNeighbor(Location& neighbor, int travelTimeSec)
{
	m_neighbors.push_back(&neighbor);
	m_travelTimes.push_back(travelTimeSec);
}





Area::Area(const wchar_t* szName, int nDanger)
{
	m_cRef = 1;

	m_pProperties = new VariableCollection();

	m_name = szName;

	VLVariable* v = m_pProperties->NewVariable(L"contents");

	

	Placeable* plc = new Placeable();
	m_contents->Add(plc, L"", 0);
	v->SetInterface(m_contents, CLASSID::INTERFACELIST);

	plc->Release();
	m_nDangerLevel = nDanger;
	m_pQuest = new Quest(L"Gather Herbs at the Forest Edge", 1, 0);
}

Area::~Area()
{
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

HRESULT __stdcall Area::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
	return S_OK;
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
	MoeInf<ICREATURE, CLASSID::CREATURE> iCrt;
	HRESULT hr = S_OK;
	
	while (m_contents->ForEach(iCrt) == S_OK)
	{
		iCrt.GetInterface();

		if ((ICREATURE*)iCrt != NULL)
		{
			if (FAILED(iCrt->Tick(nSec)))
			{
				hr = E_FAIL;
			}
		}			
	}

	return hr;
}

Placeable& Area::GetPlaceable(int ndx)
{
	Placeable* pPlc = NULL;
	m_contents->Get(ndx, (IUnknown**)&pPlc);
	return *pPlc;
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

Location& Area::AddLocation(const wchar_t* szCmd)
{
	Location* pLoc = new Location(szCmd);
	m_locations.Add(pLoc, L"", 0);
	pLoc->Release();
	return *pLoc;
}

void Area::LinkLocations(Location& a, Location& b, int travelTimeSec)
{
	a.AddNeighbor(b, travelTimeSec);
	b.AddNeighbor(a, travelTimeSec);
}

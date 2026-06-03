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




Location::Location()
{

}

Location::~Location()
{

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
	m_locations[0] = NULL;
}

Area::~Area()
{
	m_pProperties->Release();
	delete m_pQuest;

	int i = 0;

	while (i < 5 && m_locations[i] != NULL)
	{
		delete m_locations[i];
		i++;
	}	
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
	m_pProperties->Save(&tbl);

}

void Area::Load(Table& tbl)
{
	m_pProperties->Load(&tbl);
}

Location& Area::AddLocation()
{
	int i = 0;

	while (i < 5 && m_locations[i] != NULL)
		i++;

	if (i == 5)  // TODO  this will be a collection list eventually, for now just a straight array
		return *m_locations[4];

	m_locations[i] = new Location();

	if (i < 4)
		m_locations[i + 1] = NULL;

	return *m_locations[i];
}

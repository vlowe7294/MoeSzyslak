#include "pch.h"
#include "Area.h"

Area::Area()
{
	m_cRef = 1;

	m_pProperties = new VariableCollection();

	m_pName = m_pProperties->NewVariable(L"name");
	VLVariable* v = m_pProperties->NewVariable(L"contents");

	

	Placeable* plc = new Placeable();
	m_contents->Add(plc, L"", 0);
	v->SetInterface(m_contents, CLASSID::INTERFACELIST);

	plc->Release();
}

Area::~Area()
{
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

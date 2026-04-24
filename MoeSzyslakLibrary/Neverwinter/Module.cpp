#include "../pch.h"
#include "Module.h"

Module::Module()
{
	Area* pArea = new Area();

	m_cRef = 1;
	m_pProperties = new VariableCollection();
	m_pName = m_pProperties->NewVariable(L"Module Name");
	m_pName->SetString(L"New Module");

	m_pAreasVar = m_pProperties->NewVariable(L"Areas");
	m_pAreas = new InterfaceCollection();
	
	m_pAreas->Add(pArea, L"", 0);
	pArea->Release();
	m_pAreasVar->SetInterface((IUnknown*)m_pAreas, UserInf::ClassID);
}

Module::~Module()
{
	m_pProperties->Release();
	m_pAreas->Release();
}

HRESULT __stdcall Module::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == MODULE_IID)
	{
		*ppvObj = static_cast<IMODULE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Module::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Module::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Module::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
	return S_OK;
}

HRESULT __stdcall Module::Command(const wchar_t* szCmd, IUnknown* iRetStr)
{
	VLStringCollection wrds;
	StringInf iStr;
	int i = 0;
	MoeInf<IAREA, CLASSID::AREA> iArea;

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
	else if (wrds.Compare(0, L"area"))
	{
		i = _wtoi(wrds.Get(1).c_str());
		m_pAreas->Get(i, iArea);

		if ((IUnknown*)iArea == NULL)
		{
			iStr->Set(L"Invalid area index");
			return E_FAIL;
		}
		else
		{
			return iArea->Command(wrds.SubString(2, L' ').c_str(), iRetStr);
		}
	}

	iStr->Set(L"Unknown Command");
	return E_FAIL;
	
}

HRESULT __stdcall Module::Save(IUnknown* iunk)
{
	Table* pTbl = NULL;
	int n = 0;
	MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
	
	iDB.Attach(iunk);
	iDB->GetTable(L"modules", (IUnknown**)&pTbl);
	m_pProperties->Save(pTbl);

	iDB->GetTable(L"areas", (IUnknown**)&pTbl);
	Area* pArea = NULL;

	while (m_pAreas->ForEach((IUnknown**)&pArea))
	{
		if (n > 0)
			pTbl->NewRow();

		pArea->Save(*pTbl);
		n++;
	}

	return S_OK;
}

void Module::Load(Database& db)
{
	Table* pTbl = NULL;
	Area* pArea = NULL;
	int nRows = 0;

	db.GetTable(L"modules", (IUnknown**)&pTbl);
	m_pProperties->Load(pTbl);
	m_pAreas->Clear();

	db.GetTable(L"areas", (IUnknown**)&pTbl);

	nRows = pTbl->RowCount();
	pTbl->GoToTopRow();

	for (int i = 0; i < nRows; i++)
	{
		Area* pArea = new Area();
		pArea->Load(*pTbl);
		m_pAreas->Add(pArea, L"", 0);

		pArea->Release();
		pTbl->NextRow();
	}
	

}
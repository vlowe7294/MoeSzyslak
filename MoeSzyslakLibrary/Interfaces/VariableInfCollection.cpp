#include "../pch.h"
#include "../Interfaces.h"
#include "../VLVariable.h"

__declspec(dllexport) void CreateMoeSzyslakInterface(UINT nClassID, IUnknown** iunk);

GUID VariableInf::m_iid =
{ 0xd383c87b, 0x63dd, 0x4875, { 0xbc, 0x77, 0x74, 0xb0, 0x1f, 0x11, 0x22, 0x6b } };

VariableInf::VariableInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

VariableInf::~VariableInf()
{
	Dispose();
}

void VariableInf::Init()
{
	m_iUnk = NULL;
	Dispose();

	CreateMoeSzyslakInterface(ClassID, &m_iUnk);

	if (m_iUnk == NULL)
		return;

	GetInterface();
	m_iUnk->Release();
}

void VariableInf::Dispose()
{
	if (m_iVar != NULL)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}
}

VariableInf::IVARIABLE* VariableInf::operator->()
{
	if (m_iUnk == NULL)
		Init();

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();

	return m_iVar;
}

void VariableInf::GetInterface()
{
	m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
}

void VariableInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;

	if (m_iUnk == NULL)
	{
		Init();
		return;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();
}

bool VariableInf::Verify()
{
	if (m_iUnk == NULL)
		return false;

	if (m_iVar == NULL)
		GetInterface();

	return m_iVar != NULL;
}

const wchar_t* VariableInf::GetString()
{
	static std::wstring ret;
	wchar_t buf[255];
	int i = 0;

	ret.clear();
	Attach(m_iUnk);

	if (m_iVar == NULL)
		return ret.c_str();

	m_iVar->GetString(m_str);
	m_str->Get(buf, 0, 250);

	while (buf[0] > L'\0')
	{
		ret += buf;
		i += 250;
		m_str->Get(buf, i, 250);
	}

	return ret.c_str();
}

void VariableInf::SetString(const wchar_t* szStr)
{
	Attach(m_iUnk);

	if (m_iVar == NULL)
		return;

	m_iVar->SetString(szStr);
}

void VariableInf::SetInt(int nVal, int ndx)
{
	Attach(m_iUnk);

	if (m_iVar == NULL)
		return;

	m_iVar->SetAsInt(nVal, ndx);

}






VariableInfCollection::VariableInfCollection(IUnknown* iunk)
{
	m_iCollection.Attach(iunk);
	m_iUnk = iunk;
	m_iLastVariable = NULL;
}

VariableInfCollection::~VariableInfCollection()
{
	
}

std::wstring VariableInfCollection::Get(std::wstring nme)
{
	VLVariable* iVar = NULL;
	StringInf iStr;

	Get(nme, (IUnknown**)&iVar);

	if (iVar == NULL)
		return L"";

	iVar->GetString(iStr);
	return wstring(iStr);
}

void VariableInfCollection::GetVariableInterface(std::wstring nme, IUnknown** iunk)
{
	VLVariable* iVar = NULL;

	Get(nme, (IUnknown**)&iVar);
	*iunk = NULL;

	if (iVar == NULL)
		return;

	iVar->GetInterface(iunk);

}

void VariableInfCollection::Get(std::wstring nme, IUnknown** iVar)
{
	*iVar = NULL;

	Attach();

	m_iCollection->GetByTag(nme.c_str(), iVar);
}

void VariableInfCollection::Set(std::wstring nme, std::wstring val)
{
	VLVariable* iVar = NULL;
	StringInf iStr;

	Get(nme, (IUnknown**)&iVar);

	if (iVar == NULL)
		return;

	iVar->SetString(val.c_str());
}

InterfaceCollectionInf::IINTERFACECOLLECTION* VariableInfCollection::operator->()
{
	if (m_iUnk == NULL)
		throw std::runtime_error("Invalid Interface");

	Attach();

	return m_iCollection;
}

bool VariableInfCollection::Edit(std::wstring& prmpt, std::wstring val)
{
	VariableInf iVar;
	StringInf iStr;

	Attach();
	
	if (m_iLastVariable != NULL)
	{
		iVar.Attach(m_iLastVariable);
		iVar->SetString(val.c_str());
	}

	if (m_iCollection->ForEach(iVar) != S_OK)
	{
		m_iLastVariable = NULL;
		prmpt.clear();
		return false;
	}

	iVar->GetDisplayName(iStr);
	prmpt = (const wchar_t*)iStr;
	m_iLastVariable = iVar;
	return true;
}

void VariableInfCollection::Attach()
{
	m_iCollection.Attach(m_iUnk);
	m_iLastVariable = NULL;

}

bool VariableInfCollection::IsEmpty()
{
	int nCnt = 0;
	m_iCollection->Count(&nCnt);
	return nCnt < 1;
}

bool VariableInfCollection::GetBool(std::wstring nme)
{
	BOOL b = FALSE;
	VLVariable* iVar = NULL;
	
	Get(nme, (IUnknown**)&iVar);

	if (iVar == NULL)
		return false;

	iVar->GetAsBool(&b);
	return b == TRUE;
}

void VariableInfCollection::SetBool(std::wstring nme, bool bVal)
{
	VLVariable* iVar = NULL;
	
	Get(nme, (IUnknown**)&iVar);

	if (iVar == NULL)
		return;

	if (bVal)
		iVar->SetAsBool(TRUE);
	else
		iVar->SetAsBool(FALSE);

}

bool VariableInfCollection::ForEach(IUnknown** iunk)
{
	Attach();

	return m_iCollection->ForEach(iunk) == S_OK;
}
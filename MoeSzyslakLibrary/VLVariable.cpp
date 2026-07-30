#include "pch.h"
#include "VLVariable.h"
#include <algorithm>
#include "Utilities/Database.h"
#include "Testing.h"

void _cdecl DestroyMoeSzyslakHandle(UINT hObj);
UINT AddHandle(IUnknown* iunk, UINT nClassID);

VLVariable::VLVariable()
{
	static int nID = 0;
	nID++;
	m_nID = nID;

	m_cRef = 1;
	m_bLocked = false;
	m_pValues = new double[1];
	m_pValues[0] = 0.0;

	m_pOrigValues = new double[1];
	m_pOrigValues[0] = 0.0;
	m_type = TYPE_STRING;

	m_limits[0] = 0;
	m_limits[1] = 0;
	// m_iStr = new VLString();
	m_iStr.Init();

	m_bCanBeEmpty = true;
	m_iDate = NULL;
	m_nMaxSize = 1;
	m_iunk = NULL;
	m_iDisplayName.Init();
	m_iReturnUnk = m_iStr;
	m_iReturnStr.Init();
	m_iOrigDate = NULL;	
}

VLVariable::~VLVariable()
{
	if (m_iDate != NULL)
		m_iDate->Release();

	if (m_iOrigDate != NULL)
		m_iOrigDate->Release();

	if (m_iunk != NULL)
	{
		m_iunk->Release();
		DestroyMoeSzyslakHandle((UINT)m_pValues[0]);
	}

	delete m_pValues;
	delete m_pOrigValues;
}

HRESULT __stdcall VLVariable::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == VariableInf::m_iid)
	{
		*ppvObj = static_cast<VariableInf::IVARIABLE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;
}

ULONG __stdcall VLVariable::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall VLVariable::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall VLVariable::SetAsBool(BOOL bVal)
{
	SetLimits(0, 1);

	if (m_bLocked)
		return E_FAIL;

	if (bVal == TRUE)
	{
		m_iStr->Set(L"TRUE");

		if (m_pValues[0] != 1.0)
			m_pOrigValues[0] = m_pValues[0];

		m_pValues[0] = 1.0;
	}
	else
	{
		m_iStr->Set(L"FALSE");

		if (m_pValues[0] != 0.0)
			m_pOrigValues[0] = m_pValues[0];

		m_pValues[0] = 0.0;
	}

	m_type = TYPE_BOOL;
	return S_OK;
}

HRESULT __stdcall VLVariable::GetAsBool(BOOL* bVal)
{
	if (m_pValues[0] > 0.0)
		*bVal = TRUE;
	else
		*bVal = FALSE;

	return S_OK;
}

IUnknown* GetInterfaceByHandle(UINT hObj);

HRESULT __stdcall VLVariable::Command(const wchar_t* szCmd)
{
	HRESULT hr = E_FAIL;
	VLStringCollection cmds;	
	wchar_t buf[255];
	UINT h;
	wstring cmd;
	MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
	IUnknown* iTBL = NULL;
	Database* pDB = NULL;

	m_iReturnUnk = NULL;

	if (m_type == TYPE_INTERFACE && m_iunk != NULL)
	{
		UINT hObj = (UINT)m_pValues[0];
		m_iReturnStr->Set(buf);
		m_iReturnUnk = m_iReturnStr;
		
		return S_OK;
	}

	cmds.Split(szCmd, L' ');

	switch (szCmd[0])
	{
	case L'g':
		if (cmds.Compare(0, L"get"))
		{
			if (cmds.Compare(1, L"type"))
			{
				hr = m_iReturnStr->Set(std::to_wstring((int)m_type).c_str());
				m_iReturnUnk = m_iReturnStr;
			}
			else if (cmds.Compare(1, L"value"))
			{				
				hr = m_iReturnStr->Set(GetString().c_str());
				m_iReturnUnk = m_iStr;
			}
			else if (cmds.Compare(1, L"display"))
			{
				m_iReturnUnk = m_iDisplayName;
				hr = S_OK;
			}
		}
		break;

	case L's':
		if (cmds.Compare(0, L"set"))
		{
			if (cmds.Compare(1, L"display"))
			{
				hr = m_iDisplayName->Set(cmds.Get(2).c_str());
			}
			else if (cmds.Compare(1, L"value"))
			{
				hr = SetString(cmds.Get(2).c_str());
			}
			else if (cmds.Compare(1, L"type"))
			{
				hr = SetType(_wtoi(cmds.Get(2).c_str()));
			}
		}
		else if (cmds.Compare(0, L"save"))
		{
			h = _wtoi(cmds.Get(1).c_str());
			iDB.Attach(GetInterfaceByHandle(h));

			if ((IDATABASE*)iDB != NULL)
			{
				iDB->GetTable(cmds.Get(2).c_str(),  &iTBL);
				hr = Save(iTBL);
			}
		}
		break;
	}

	return hr;
}

HRESULT __stdcall VLVariable::GetReturnString(IUnknown** iStr)
{
	*iStr = m_iReturnUnk;
	return S_OK;
}

HRESULT __stdcall VLVariable::GetType(UINT* nType)
{
	*nType = m_type;
	return S_OK;
}

HRESULT __stdcall VLVariable::SetType(UINT nType)
{
	if (m_bLocked)  // variable value is locked can't change
		return E_FAIL;

	if (m_type == nType)  // type is the same, nothing to do
		return S_OK;	

	wstring v = (const wchar_t*)m_iStr;	

	if (m_type == TYPE_BOOL)
		m_limits[0] = m_limits[1] = 0;

	switch (nType)
	{
	case TYPE_STRING:
		m_type = TYPE_STRING;
		break;

	case TYPE_DATE:
		GetDateTime();		
		break;

	case TYPE_INT:		
		SetAsInt(0, 0);
		break;

	case TYPE_BOOL:
		SetAsBool(FALSE);
		break;

	case TYPE_INTERFACE:
	case TYPE_COLLECTION:
		SetInterface(NULL, 0);
		break;

	case TYPE_FLOAT:
		SetAsFloat(0, 0.0);
		break;
	}

	SetString(v.c_str());
	return S_OK;
}

HRESULT __stdcall VLVariable::GetDisplayName(IUnknown** iNmeStr)
{
	m_iDisplayName->QueryInterface(IID_IUnknown, (void**)iNmeStr);
	m_iDisplayName->Release();
	return S_OK;
}

HRESULT __stdcall VLVariable::UnitTest()
{
	VLVariable* pCpy = new VLVariable();
	StringInf istr;	
	Database* pDB = new Database();
	Table* pTBL = NULL;
	BOOL bVal = FALSE;

	VLDateTime& dt = pCpy->GetDateTime();

	NeverwinterInf iNWN;

	iNWN.Attach();
	pCpy->SetInterface(iNWN, NeverwinterInf::ClassID);
	pCpy->Command(L"module get value");
	pCpy->GetReturnString(istr);

	pDB->GetTable(L"test_variables", (IUnknown**)&pTBL);
	Save(pTBL);
	pDB->Save(L"variables.db");
	pDB->Release();

	pDB = new Database();
	pDB->Load(L"variables.db");

	Copy(*pCpy);
	pCpy->HasChanged(&bVal);
	pCpy->Release();
	pDB->Release();
	
	return S_OK;
}

HRESULT __stdcall VLVariable::SetString(const wchar_t* szVal)
{
	wstring orig, strVal(szVal);
	VLStringCollection col;
	int i = 0;

	if (m_bLocked)
		return E_FAIL;

	if (!m_bCanBeEmpty && strVal.length() == 0)
		return E_FAIL;

	switch (m_type)
	{
	case TYPE_BOOL:
		std::transform(strVal.begin(), strVal.end(), strVal.begin(), std::tolower);

		if (strVal == L"true")
			SetAsBool(TRUE);
		else
			SetAsBool(FALSE);

		break;

	case TYPE_INT:
		if (strVal.find(L',') != wstring::npos)
		{
			col.Split(strVal.c_str(), L',');
			while (col.ForEach(strVal))
				SetAsInt(_wtoi(strVal.c_str()), i++);
		}
		else
			SetAsInt(_wtoi(strVal.c_str()), 0);

		break;

	default:
		m_iStr.BufferSize((UINT)strVal.length());
		orig = (const wchar_t*)m_iStr;

		if (orig != strVal)
		{
			m_iStr->Set(strVal.c_str());
		}
		break;
	}

	return S_OK;
}

HRESULT __stdcall VLVariable::GetString(IUnknown** iStr)
{
	m_iStr->QueryInterface(IID_IUnknown, (void**)iStr);
	m_iStr->Release();
	return S_OK;
}

HRESULT __stdcall VLVariable::SetAsInt(int nVal, int ndx)
{
	wstring s;

	if (m_bLocked)
		return E_FAIL;

	if (ndx < 0)
		return E_FAIL;

	if (m_type == TYPE_BOOL && ndx == 0)
	{
		if (nVal > 0)
			SetAsBool(TRUE);
		else
			SetAsBool(FALSE);

		return S_OK;
	}


	if (m_limits[1] == m_limits[0] || (nVal >= m_limits[0] && nVal <= m_limits[1]))
	{
		GrowSize(ndx);

		if ((int)m_pValues[ndx] != nVal || m_type != TYPE_INT)
		{
			m_pOrigValues[ndx] = m_pValues[ndx];
			m_pValues[ndx] = nVal;

			wstring orig = (LPCWSTR)m_iStr;

			for (int i = 0; i <= ndx; i++)
			{
				if (i > 0)
					s += L",";

				s += std::to_wstring((int)m_pValues[i]);
				m_iStr->Set(s.c_str());
			}
		}

		m_type = TYPE_INT;
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT __stdcall VLVariable::GetAsInt(int ndx, int* nVal)
{
	*nVal = 0;

	if (ndx >= 0 && (UINT)ndx < m_nMaxSize)
	{
		*nVal = (int)m_pValues[ndx];
	}

	return S_OK;
}

HRESULT __stdcall VLVariable::SetAsFloat(int ndx, double fVal)
{
	wstring s;

	if (m_bLocked)
		return E_FAIL;

	if (ndx < 0)
		return E_FAIL;

	if (m_type == TYPE_BOOL && ndx == 0)
	{
		SetAsBool(fVal > 0.0);
		return S_OK;
	}

	GrowSize(ndx);

	if (m_limits[1] == m_limits[0] || (fVal >= m_limits[0] && fVal <= m_limits[1]))
	{
		if (m_pValues[ndx] != fVal || m_type != TYPE_FLOAT)
		{
			m_pOrigValues[ndx] = m_pValues[ndx];
			m_pValues[ndx] = fVal;

			wstring orig = (LPCWSTR)m_iStr;

			for (int i = 0; i <= ndx; i++)
			{
				if (i > 0)
					s += L",";

				s += std::to_wstring(m_pValues[i]);
				m_iStr->Set(s.c_str());
			}
		}

		m_type = TYPE_FLOAT;
		return S_OK;
	}
	else
		return E_FAIL;

}

HRESULT __stdcall VLVariable::GetAsFloat(int ndx, double* fVal)
{
	*fVal = 0.0;

	if (ndx >= 0 && ndx < (int)m_nMaxSize)
		*fVal = m_pValues[ndx];

	return S_OK;
}

HRESULT __stdcall VLVariable::SetInterface(IUnknown* iunk, UINT nClassID)
{
	if (m_bLocked)
		return E_FAIL;

	if (m_iunk != NULL)
	{
		m_iunk->Release();
	}

	m_type = TYPE_INTERFACE;
	m_iunk = iunk;
	m_pValues[0] = nClassID;

	if (m_iunk != NULL)
	{
		m_iunk->AddRef();
	}

	m_iStr->Set(L"");
	return S_OK;
}

HRESULT __stdcall VLVariable::GetInterface(IUnknown** iunk)
{
	*iunk = m_iunk;

	if (m_iunk == NULL)
		return E_FAIL;
	else
		return S_OK;

}

HRESULT __stdcall VLVariable::GetLocked(BOOL* bLocked)
{
	if (m_bLocked)
		*bLocked = TRUE;
	else
		*bLocked = FALSE;

	return S_OK;
}

HRESULT __stdcall VLVariable::SetLocked(BOOL bLocked)
{
	m_bLocked = (bLocked == TRUE);
	return S_OK;
}

HRESULT __stdcall VLVariable::Save(IUnknown* iTbl)
{
	Table* pTbl = (Table*)iTbl;

	pTbl->Set((const wchar_t*)m_iDisplayName, (const wchar_t*)m_iStr, m_type);
	return S_OK;
}

HRESULT __stdcall VLVariable::Copy(IUnknown* iVar)
{
	VariableInf cpyFrm;
	VLVariable* pVar;

	cpyFrm.Attach(iVar);

	if (!cpyFrm.Verify())
		return E_FAIL;

	pVar = (VLVariable*)((IUnknown*)cpyFrm);

	if (m_bLocked)
		return E_FAIL;

	m_type = pVar->m_type;
	m_iDisplayName->Set(pVar->m_iDisplayName);

	if (m_type == TYPE_DATE)
	{
		if (m_type == TYPE_DATE && m_iDate == NULL)
			m_iDate = new VLDateTime();

		VLDateTime& t = pVar->GetDateTime();
		m_iDate->Copy(t);
	}

	m_iStr->Set((const wchar_t*)pVar->m_iStr);
	GrowSize(pVar->m_nMaxSize - 1);
	memcpy(m_pValues, pVar->m_pValues, sizeof(double) * pVar->m_nMaxSize);
	return S_OK;
}

HRESULT __stdcall VLVariable::HasChanged(BOOL* bChanged)
{
	*bChanged = FALSE;

	switch (m_type)
	{
	case TYPE_DATE:

		if (m_iOrigDate == NULL && m_iDate != NULL)
			return true;

		if (m_iOrigDate->Compare(*m_iDate))
			*bChanged = TRUE;

	case TYPE_INT:
		if (memcmp(m_pOrigValues, m_pValues, sizeof(double) * m_nMaxSize) != 0)
			*bChanged = TRUE;
		break;

	case TYPE_BOOL:
		if (m_pOrigValues[0] != m_pValues[0])
			*bChanged = TRUE;
		break;

	default:
		if (m_origValue != wstring((LPCWSTR)m_iStr))
			*bChanged = TRUE;
	}

	return S_OK;
}

HRESULT __stdcall VLVariable::Compare(IUnknown* iunk, int* nCmp)
{
	VariableInf iVar;
	iVar.Attach(iunk);

	if ((IVARIABLE*)iVar == NULL)
		return E_FAIL;

	VLVariable* pVar = (VLVariable*)((IVARIABLE*)iVar);
	*nCmp = 0;
	
	int nSize = m_nMaxSize;

	if (m_type == TYPE_INT)
	{
		if (pVar->m_nMaxSize < (UINT)nSize)
			nSize = pVar->m_nMaxSize;

		*nCmp = memcmp(m_pValues, pVar->m_pValues, sizeof(double) * nSize);
	}
	else
	{
		wstring str1((const wchar_t*)m_iStr);
		*nCmp = str1.compare((const wchar_t*)pVar->m_iStr);
	}

	return S_OK;
}


void VLVariable::Load(IUnknown* iTbl)
{
	Table* pTbl = (Table*)iTbl;
	SetString(pTbl->Get((const wchar_t*)m_iDisplayName).c_str());
}


void VLVariable::SaveToFile(VLFile& fle)
{
	fle.Write(m_type);
	fle.Write(wstring(m_iStr));
}

void VLVariable::LoadFromFile(VLFile& fle)
{
	int n = 0;
	VLString s;
	fle.Read(n);
	fle.Read(s);

	m_type = (VAR_TYPE)n;
	m_iStr->Set(s.Get().c_str());
}


void VLVariable::SetLimits(int nMin, int nMax)
{
	if (nMax == nMin)  // no limits
	{
		m_limits[0] = 0;
		m_limits[1] = 0;
	}
	else if (nMax > nMin)  // nMax < nMin is invalid
	{
		m_limits[0] = nMin;
		m_limits[1] = nMax;
	}
}

wstring VLVariable::GetString()
{
	return (const wchar_t*)m_iStr;
}



void VLVariable::GrowSize(int ndx)
{
	double* pNew = NULL;

	while ((UINT)ndx >= m_nMaxSize)
	{
		pNew = new double[m_nMaxSize * 10];
		memset(pNew, 0, sizeof(double) * m_nMaxSize * 10);
		memcpy(pNew, m_pValues, sizeof(double) * m_nMaxSize);

		delete [] m_pValues;
		m_pValues = pNew;

		pNew = new double[m_nMaxSize * 10];
		memset(pNew, 0, sizeof(double) * m_nMaxSize * 10);
		memcpy(pNew, m_pOrigValues, sizeof(double) * m_nMaxSize);

		delete [] m_pOrigValues;
		m_pOrigValues = pNew;
		m_nMaxSize = m_nMaxSize * 10;
	}
}

VLDateTime& VLVariable::GetDateTime()
{
	m_type = TYPE_DATE;

	if (m_iDate == NULL)
		m_iDate = new VLDateTime();

	return *m_iDate;
}

void VLVariable::Copy(VLVariable& cpyFrm)
{
	if (m_bLocked)
		return;

	m_type = cpyFrm.m_type;
	m_iDisplayName->Set(cpyFrm.m_iDisplayName);

	if (m_type == TYPE_DATE)
	{
		if (m_type == TYPE_DATE && m_iDate == NULL)
			m_iDate = new VLDateTime();

		VLDateTime& d = cpyFrm.GetDateTime();
		m_iDate->Copy(&d);
	}

	m_iStr->Set((const wchar_t*)cpyFrm.m_iStr);
	GrowSize(cpyFrm.m_nMaxSize - 1);
	memcpy(m_pValues, cpyFrm.m_pValues, sizeof(double) * cpyFrm.m_nMaxSize);
}

void VLVariable::TurnOffChanged()
{
	switch (m_type)
	{
	case TYPE_DATE:

		if (m_iOrigDate == NULL && m_iDate != NULL)
			m_iOrigDate = new VLDateTime();

		m_iOrigDate->Copy(*m_iDate);
		m_origValue = wstring((LPCWSTR)m_iStr);
		break;

	case TYPE_INT:
	case TYPE_FLOAT:
		memcpy(m_pOrigValues, m_pValues, sizeof(double) * m_nMaxSize);
		m_origValue = wstring((LPCWSTR)m_iStr);
		break;

	case TYPE_BOOL:
		m_pOrigValues[0] = m_pValues[0];
		m_origValue = wstring((LPCWSTR)m_iStr);
		break;

	default:
		m_origValue = wstring((LPCWSTR)m_iStr);
		break;
	}

}







VariableCollection::VariableCollection()
{
	g_memoryChecker.IncrementInstance(CLASSID::VARIABLECOLLECTION);
	m_cRef = 1;

}

VariableCollection::~VariableCollection()
{
	g_memoryChecker.DecrementInstance(CLASSID::VARIABLECOLLECTION);
}

HRESULT __stdcall VariableCollection::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == VARIABLELIST_IID)
	{
		*ppvObj = static_cast<IVARIABLELIST*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;
}

ULONG __stdcall VariableCollection::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall VariableCollection::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall VariableCollection::GetAsString(const wchar_t* szTag, BSTR* bsStr)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag);
	*bsStr = NULL;
	
	if (pVar != NULL)
	{
		*bsStr = SysAllocString(pVar->GetString().c_str());
		pVar->Release();		
	}

	return S_OK;
}

HRESULT __stdcall VariableCollection::SetAsString(const wchar_t* szTag, const wchar_t* szVal)
{
	VLVariable* pVar = NULL;
	StringInf iStr;

	pVar = (VLVariable*)m_variables.GetByTag(szTag);

	if (pVar == NULL)
	{
		pVar = new VLVariable();
		pVar->GetDisplayName(iStr);
		iStr->Set(szTag);

		pVar->SetType(VLVariable::TYPE_STRING);
		m_variables.Add(pVar, szTag, 0);
	}

	pVar->SetString(szVal);
	pVar->Release();
	return S_OK;
}

HRESULT __stdcall VariableCollection::SetAsInt(const wchar_t* szTag, int nVal, int ndx)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag);

	if (pVar == NULL)
	{
		pVar = NewVariable(szTag);
		pVar->AddRef();
	}	
	
	pVar->Release();
	return pVar->SetAsInt(nVal, ndx);
}

HRESULT __stdcall VariableCollection::GetAsBool(const wchar_t* szTag, BOOL* bVal)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag);
	*bVal = FALSE;

	if (pVar != NULL)
	{
		pVar->GetAsBool(bVal);
		pVar->Release();
	}

	return S_OK;
}

HRESULT __stdcall VariableCollection::SetAsBool(const wchar_t* szTag, BOOL bVal)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag);

	if (pVar == NULL)
	{
		pVar = NewVariable(szTag);
		pVar->AddRef();
	}

	pVar->Release();
	return pVar->SetAsBool(bVal);
}

HRESULT __stdcall VariableCollection::GetAsInt(const wchar_t* szTag, int* nVal)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag);
	*nVal = 0;

	if (pVar != NULL)
	{
		pVar->GetAsInt(0, nVal);
		pVar->Release();
	}

	return S_OK;

}

VLVariable* VariableCollection::NewVariable(wstring szTag)
{
	VLVariable* pVar = new VLVariable();	
	StringInf iStr;

	pVar->GetDisplayName(iStr);
	iStr->Set(szTag.c_str());


	m_variables.Add(pVar, szTag.c_str(), 0);
	pVar->Release();

	return pVar;

}

void VariableCollection::Set(wstring szTag, wstring szValue, VLVariable::VAR_TYPE type)
{
	VLVariable* pVar = NULL;
	StringInf iStr;

	pVar = (VLVariable*)m_variables.GetByTag(szTag.c_str());

	if (pVar == NULL)
	{
		pVar = new VLVariable();
		pVar->GetDisplayName(iStr);
		iStr->Set(szTag.c_str());

		pVar->SetType(type);
		m_variables.Add(pVar, szTag.c_str(), 0);		
	}

	pVar->SetString(szValue.c_str());
	pVar->Release();
}

wstring VariableCollection::Get(wstring szTag)
{
	VLVariable* pVar = (VLVariable*)m_variables.GetByTag(szTag.c_str());

	if (pVar != NULL)
	{
		pVar->Release();
		return pVar->GetString();
	}
	else
		return L"";

}

void VariableCollection::Save(IUnknown* iTbl)
{
	VLVariable* pVar = NULL;
	
	while (ForEach(&pVar))
	{
		pVar->Save(iTbl);
	}

}

void VariableCollection::Load(IUnknown* iTbl)
{
	Table* pTbl = (Table*)iTbl;

	wstring c = pTbl->GetColumn(0);

		
	

}

bool VariableCollection::ForEach(VLVariable** pVar)
{
	*pVar = NULL;
	bool b = m_variables.ForEach((IUnknown**)pVar);

	if (b && *pVar != NULL)
		(*pVar)->Release();

	return b;
}

VLVariable* VariableCollection::Get(int ndx)
{
	VLVariable* pVar = (VLVariable*)m_variables.Get(ndx);

	if (pVar != NULL)
		pVar->Release();

	return pVar;
}

void VariableCollection::TurnOffChanged()
{
	VLVariable* pVar = NULL;

	while (ForEach(&pVar))
	{
		pVar->TurnOffChanged();
	}

}

bool VariableCollection::HasChanged()
{
	VLVariable* pVar = NULL;
	BOOL bChanged = FALSE;

	while (ForEach(&pVar))
	{
		pVar->HasChanged(&bChanged);

		if (bChanged == TRUE)
		{
			m_variables.Reset();
			return true;
		}
	}

	return false;

}

VLVariable* VariableCollection::GetByTag(const wchar_t* szTag)
{
	VLVariable* v = (VLVariable*)m_variables.GetByTag(szTag);

	if (v != NULL)
		v->Release();

	return v;
}

void VariableCollection::Clear()
{
	m_variables.Clear();
}

int VariableCollection::Count()
{
	return m_variables.Count();
}

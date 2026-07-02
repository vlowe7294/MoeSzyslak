#include "pch.h"
#include "VLString.h"

InterfaceCollection::InterfaceCollection()
{
	m_cRef = 1;
	m_nObjects = 0;
	m_maxObjects = 100;
	m_array = new IUnknown*[100];
	m_nCurrent = 0;
	m_iLocalStr.Init();
	
}

InterfaceCollection::~InterfaceCollection()
{
	Clear();
}


HRESULT __stdcall InterfaceCollection::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == InterfaceCollectionInf::m_iid)
	{
		*ppvObj = static_cast<IINTERFACECOLLECTION*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall InterfaceCollection::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall InterfaceCollection::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall InterfaceCollection::Add(IUnknown* iObj, const wchar_t* szTag, UINT nClassID)
{
	std::wstring tag(szTag);

	if (m_nObjects == m_maxObjects)
	{
		IUnknown** pNew = new IUnknown*[m_maxObjects * 2];

		for (int i = 0; i < m_nObjects; i++)
		{
			pNew[i] = m_array[i];
			m_array[i] = NULL;
		}

		delete m_array;
		m_array = pNew;
		m_maxObjects = m_maxObjects * 2;
	}

	m_array[m_nObjects] = iObj;

	if (iObj != NULL)
		iObj->AddRef();

	m_classIDs[m_nObjects] = nClassID;
	
	if (tag.length() > 0)
	{
		m_mapToIndex[tag] = m_nObjects;
		m_mapToTag[m_nObjects] = tag;
	}

	m_nObjects++;
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::Get(int ndx, IUnknown** iunk)
{
	*iunk = NULL;

	if (ndx >= 0 && ndx < m_nObjects)
		*iunk = m_array[ndx];

	if (*iunk == NULL)
		return E_FAIL;
	else 
		return S_OK;
}

HRESULT __stdcall InterfaceCollection::Count(int* nCnt)
{
	*nCnt = m_nObjects;
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::Remove(int ndx)
{
	if (ndx < 0 || ndx >= m_nObjects)
		return E_FAIL;

	if (m_mapToTag.find(ndx) != m_mapToTag.end())
	{
		std::wstring tag = m_mapToTag[ndx];
		m_mapToTag.erase(ndx);
		m_mapToIndex.erase(tag);
	}

	if (m_array[ndx] != NULL)
	{
		m_array[ndx]->Release();
		m_array[ndx] = NULL;
	}

	return S_OK;
}

HRESULT __stdcall  InterfaceCollection::GetByTag(const wchar_t* szTag, IUnknown** iunk)
{
	*iunk = Get(szTag);
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::Clear()
{
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_array[i] != NULL)
			m_array[i]->Release();

		m_array[i] = NULL;
	}

	m_nObjects = 0;
	m_mapToIndex.clear();
	m_mapToTag.clear();
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::ForEach(IUnknown** pObj)
{
	*pObj = NULL;

	if (m_nCurrent >= m_nObjects)
	{
		m_nCurrent = 0;
		return S_FALSE;
	}

	*pObj = m_array[m_nCurrent];
	m_nCurrent++;
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::Command(const wchar_t* szCmd)
{
	IUnknown* iunk = NULL;
	TripPlannerInf itrp;
	TestingInf iTst;
	MoeInf<IINTERFACECOLLECTION, CLASSID::INTERFACELIST> iList;
	LogEntryInf iLog;
	NeverwinterInf iNvr;
	VariableInf iVar;
	UINT ndx = 0;
	int nCnt = 0;
	VLStringCollection cmds;
	HRESULT hr = E_FAIL;
	IOSInf iIOS;
	FinanceInf iFnc;
	UserInf iUsr;

	
	cmds.Split(szCmd, L' ');
	m_iRetString = NULL;

	if (cmds.Compare(0, L"get"))
	{
		if (cmds.Compare(1, L"count"))
		{
			hr = Count(&nCnt);
			m_iLocalStr->Set(std::to_wstring(nCnt).c_str());			
		}
		else
		{
			hr = GetByTag(cmds.Get(1).c_str(), iVar);
			m_iLocalStr->Set(iVar.GetString());
		}

		m_iRetString = (IUnknown*)m_iLocalStr;
		return hr;
	}
	else if (cmds.Compare(0, L"set"))
	{
		hr = GetByTag(cmds.Get(1).c_str(), iVar);
		iVar.SetString(cmds.Get(2).c_str());
		return hr;
	}
	

	ndx = _wtoi(cmds.Get(0).c_str());
	Get(ndx, &iunk);

	if (iunk == NULL)
		return hr;

	UINT nClassID = m_classIDs[ndx];	

	if (nClassID == 0)
		iVar->GetAsInt(0, (int*)&nClassID);

	wstring subCmd = cmds.SubString(1, L' ');
;

	switch (nClassID)
	{

	case VariableInf::ClassID:
		iVar.Attach(iunk);
		hr = iVar->Command(subCmd.c_str());
		iVar->GetReturnString(&m_iRetString);
		break;

	case CLASSID::INTERFACELIST:
		iList.Attach(iunk);
		hr = iList->Command(subCmd.c_str());
		iList->GetReturnString(&m_iRetString);
		break;

	case IOSInf::ClassID:
		iIOS.Attach(iunk);
		hr = iIOS->Command(subCmd.c_str());
		break;

	case UserInf::ClassID:
		iUsr.Attach(iunk);
		hr = iUsr->Command(subCmd.c_str());
		iUsr->GetReturnString(&m_iRetString);
		break;

	case FinanceInf::ClassID:
		iFnc.Attach(iunk);
		hr = iFnc->Command(subCmd.c_str());
		iFnc->GetReturnString(&m_iRetString);
		break;

	case CLASSID::TRIPPLANNER:
		itrp.Attach(iunk);
		hr = itrp->Command(subCmd.c_str());
		itrp->GetReturnString(&m_iRetString);
		break;

	case NeverwinterInf::ClassID:
		iNvr.Attach(iunk);
		hr = iNvr->Command(subCmd.c_str());
		iNvr->GetReturnString(&m_iRetString);
		break;	
	}

	return hr;
}

HRESULT __stdcall InterfaceCollection::GetReturnString(IUnknown** iStr)
{
	*iStr = m_iRetString;
	return S_OK;
}

HRESULT __stdcall InterfaceCollection::Dispose()
{
	return S_OK;
}

IUnknown* InterfaceCollection::Get(std::wstring szTag)
{
	IUnknown* iRet = NULL;

	if (m_mapToIndex.find(szTag) == m_mapToIndex.end())  // tag does not exist
		return iRet;

	int i = m_mapToIndex[szTag];

	return m_array[i];
}

HRESULT __stdcall InterfaceCollection::UnitTest(IUnknown* iunk)
{
	VariableInf iVar;
	TestingInf iTst;
	StringInf iStr;
	int nCnt = 0, nObjects = 0, nType = 0, ndx = 0;
	wstring tag, msg;
	IUnknown* iAddObj = NULL;

	iTst.Attach(iunk);
	iTst->Message(L"Interface Collection Unit Test", 0, L"");
	
	msg = L"Created " + std::to_wstring(nObjects) + L" objects of type ";
	msg += std::to_wstring(nType);
	

	for (int i = 0; i < nObjects; i++)
	{
		iAddObj = NULL;
		CreateMoeSzyslakInterface(nType, &iAddObj);
		tag = L"Test Variable " + std::to_wstring(i);

		Add(iAddObj, tag.c_str(), nType);

		if (iAddObj != NULL)
			iAddObj->Release();
	}

	iTst->Message(msg.c_str(), 0, L"");
	
	msg = L"Getting object at index " + std::to_wstring(ndx);
	iTst->Message(msg.c_str(), 0, L"");	
	iTst->VerifyHResult(Get(ndx, iVar), L"Get failed");

	Count(&nCnt);
	iTst->VerifyVariable(L"count", std::to_wstring(nCnt).c_str());

	iTst->Message(msg.c_str(), 0, L"");
	iTst->VerifyHResult(Command(iStr), L"Command Failed");

	iTst->VerifyHResult(Remove(nType), L"Remove Failed");	

	tag = L"Test Variable " + std::to_wstring(ndx);
	GetByTag(tag.c_str(), iVar);

	while (ForEach(iVar) == S_OK)
		;

	Clear();

	return S_OK;

}

void InterfaceCollection::GetLocalString(IUnknown** iStr)
{
	*iStr = (IUnknown*)m_iLocalStr;
}

void InterfaceCollection::RemoveByTag(const wchar_t* szTag)
{
	if (m_mapToIndex.find(szTag) == m_mapToIndex.end())  // tag does not exist
		return;

	Remove(m_mapToIndex[szTag]);
}





ComCollection::ComCollection()
{
	m_nCurrent = 0;

}

ComCollection::~ComCollection()
{
	Clear();
}

void ComCollection::Clear()
{
	for (IUnknown* p : m_array)
	{
		if (p)
			p->Release();
	}
	
	m_mapToIndex.clear();
	m_mapToTag.clear();
}

void ComCollection::Add(IUnknown* iObj, const wchar_t* szTag, UINT nClassID)
{
	std::wstring tag = (szTag ? szTag : L"");
	int ndx = static_cast<int>(m_array.size());

	if (iObj)
		iObj->AddRef();

	m_array.push_back(iObj);
	m_classIDs[ndx] = nClassID;

	if (tag.length() > 0)
	{
		m_mapToIndex[tag] = ndx;
		m_mapToTag[ndx] = tag;
	}
}

int ComCollection::Count()
{
	return static_cast<int>(m_array.size());
}

bool ComCollection::ForEach(IUnknown** ppObj)
{
	if (!ppObj)
		return false;

	// End of collection?
	if (m_nCurrent >= static_cast<int>(m_array.size()))
	{
		Reset();
		return false;
	}
		

	IUnknown* p = m_array[m_nCurrent++];

	if (p)
		p->AddRef();   // COM rule: caller owns the returned reference

	*ppObj = p;
	return true;
}

void ComCollection::Reset()
{
	m_nCurrent = 0;
}

IUnknown* ComCollection::Get(int ndx)
{
	IUnknown* iunk = NULL;

	if (m_array.size() > ndx)
	{
		iunk = m_array[ndx];

		if (iunk != NULL)
			iunk->AddRef();
	}

	return iunk;
}

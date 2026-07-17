#include "pch.h"
#include <windows.h>
#include <shellapi.h>
#include "CampSight.h"

wstring VersionAsString();

Trail::Trail(wstring nme, DIFFICULTY diff, double len)
{
	m_cRef = 1;
	m_name = nme;
	m_difficulty = diff;
	m_lengthMiles = len;

	wprintf(L"%s\tdifficulty:  %d\tlength:  %f miles\n", m_name.c_str(), m_difficulty, m_lengthMiles);
}

Trail::~Trail()
{
}

HRESULT __stdcall Trail::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall Trail::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG __stdcall Trail::Release()
{
	InterlockedDecrement(&m_cRef);
	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}





std::atomic<uint32_t> g_nextSiteId{ 1 };

Site::Site(double lat, double lon) : m_id(g_nextSiteId.fetch_add(1, std::memory_order_relaxed))
{
	m_cRef = 1;
	m_latitude = lat;
	m_longitude = lon;
}

Site::~Site()
{

}

HRESULT __stdcall Site::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall Site::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG __stdcall Site::Release()
{
	InterlockedDecrement(&m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

void Site::Print()
{
	wprintf(L"%s\t%f\t%f\n", m_name.c_str(), m_latitude, m_longitude);
}

wstring Site::MapLink()
{
	std::wstring url = L"https://www.google.com/maps?q=" +
		std::to_wstring(m_latitude) + L"," + std::to_wstring(m_longitude);
	return url;
}





CampArea::CampArea(wstring nme)
{
	m_cRef = 1;
	m_name = nme;
	g_memoryChecker.IncrementInstance(CLASSID::CAMPAREA);	
}

CampArea::~CampArea()
{
	g_memoryChecker.DecrementInstance(CLASSID::CAMPAREA);
}

HRESULT __stdcall CampArea::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == CAMPAREA_IID)
	{
		*ppvObj = static_cast<ICAMPAREA*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall CampArea::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG __stdcall CampArea::Release()
{
	InterlockedDecrement(&m_cRef);
	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall CampArea::GetName(BSTR* szNme)
{
	*szNme = SysAllocString(m_name.c_str());
	return S_OK;
}
HRESULT __stdcall CampArea::SetName(const wchar_t* szNme)
{
	m_name = szNme;
	return S_OK;
}

HRESULT __stdcall CampArea::GetSite(int ndx, IUnknown** iSite)
{
	*iSite = m_sites.Get(ndx);
	return S_OK;

}

void CampArea::Save(Database& db)
{
	ITABLE* iTbl = NULL;

	db.GetTable(L"camp_sites", (IUnknown**)&iTbl);
	iTbl->Set(L"area_name", m_name.c_str(), VLVariable::TYPE_STRING);

	db.Save(L"new_campsight.db");
}

void CampArea::Load(Database& db)
{
	ITABLE* iTbl = NULL;
	StringInf iStr;
	wstring s;

	iStr.Init();
	db.Load(L"campsight.db");
	db.GetTable(L"camp_sites", (IUnknown**)&iTbl);
	iTbl->Get(L"area_name", iStr);
	s = (const wchar_t*)iStr;

	if (!s.empty())
	{
		m_name = s;
	}
}

void CampArea::AddTrail(Trail* pTrail)
{
	m_trails.Add(pTrail, pTrail->GetName().c_str(), 0);
}



extern MemoryChecker g_memoryChecker;

CampSight::CampSight()
{
	m_cRef = 1;
	g_memoryChecker.IncrementInstance(CLASSID::CAMPSIGHT);
	m_pTest = new Testing();
}

CampSight::~CampSight()
{
	g_memoryChecker.DecrementInstance(CLASSID::CAMPSIGHT);
	m_pTest->Release();
}

HRESULT __stdcall CampSight::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == CAMPSIGHT_IID)
	{
		*ppvObj = static_cast<ICAMPSIGHT*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall CampSight::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG __stdcall CampSight::Release()
{
	InterlockedDecrement(&m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall  CampSight::GetTester(IUnknown** iTester)
{
	m_pTest->QueryInterface(IID_IUnknown, (void**)iTester);
	return S_OK;
}

HRESULT __stdcall CampSight::AddSite(const wchar_t* szArea, const wchar_t* szSite, double lat, double lon)
{
	CampArea* pArea = NULL;

	if (szArea == NULL || szArea[0] == L'\0' || szSite == NULL || szSite[0] == L'\0')
	{
		m_pTest->Verify(FALSE, L"CampSight::AddSite() Invalid area or site name", Testing::DEBUG_CRITICAL, L"CampSight");
		LogEntry& l = m_pTest->GetLastEntry();
		l.SetFile(L"CampSight.cpp");
		l.SetLine(__LINE__);
		return E_INVALIDARG;
	}		

	pArea = (CampArea*)m_areas.GetByTag(szArea);

	if (pArea == NULL)
	{
		pArea = new CampArea(szArea);
		m_areas.Add(pArea, szArea, 0);
	}

	Site* pSite = new Site(lat, lon);
	pSite->SetName(szSite);
	pArea->GetSites().Add(pSite, szSite, 0);

	pSite->Release();
	pArea->Release();
	return S_OK;
}

HRESULT __stdcall CampSight::GetArea(int ndx, IUnknown** iArea)
{
	*iArea = m_areas.Get(ndx);
	return S_OK;
}

HRESULT __stdcall CampSight::UnitTest()
{	
	CampArea* pArea = NULL;
	Site* pSite = NULL;

	GetArea(0, (IUnknown**)&pArea);
	pArea->GetSite(0,(IUnknown**)&pSite);
	wstring mapLink = pSite->MapLink();

	pSite->Release();
	Database* pDB = new Database();

	pArea->Save(*pDB);

	pDB->Release();
	pDB = new Database();

	pArea->Load(*pDB);
	pDB->Release();

	Trail* pTrail = new Trail(L"Black-Capped Vireo Trail", Trail::DIFFICULTY_MODERATE, 4.3);
	pArea->AddTrail(pTrail);
	pTrail->Release();

	pArea->GetSites().RemoveByTag(L"Laham Mill #14");
	pArea->Release();
	return S_OK;
}

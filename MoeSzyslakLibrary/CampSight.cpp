#include "pch.h"
#include <windows.h>
#include <shellapi.h>
#include "CampSight.h"

std::atomic<uint32_t> g_nextSiteId{ 1 };

Site::Site(wstring nme, double lat, double lon) : m_id(g_nextSiteId.fetch_add(1, std::memory_order_relaxed))
{
	m_cRef = 1;
	m_name = nme;
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






CampArea::CampArea(wstring nme)
{
	m_cRef = 1;
	m_name = nme;
}

CampArea::~CampArea()
{

}

HRESULT __stdcall CampArea::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

void CampArea::Print()
{
	int i = 0;
	Site* pSite = NULL;
	wprintf(L"%s\n", m_name.c_str());

	wprintf(L"Name\tLatitude\tLongitude\n");

	while (m_sites.ForEach((IUnknown**)&pSite) == S_OK)
	{
		i++;
		wprintf(L"%d)  ", i);
		pSite->Print();
	}
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





TestingInf CampSight::m_test;

CampSight::CampSight()
{
	m_cRef = 1;
}

CampSight::~CampSight()
{
	
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
	*iTester = (IUnknown*)m_test;
	return S_OK;
}

HRESULT __stdcall CampSight::AddSite(const wchar_t* szArea, const wchar_t* szSite, double lat, double lon)
{
	CampArea* pArea = NULL;

	if (szArea == NULL || szArea[0] == L'\0' || szSite == NULL || szSite[0] == L'\0')
	{
		m_test->Verify(FALSE, L"CampSight::AddSite() Invalid area or site name");
		return E_INVALIDARG;
	}		

	m_areas.GetByTag(szArea, (IUnknown**)&pArea);

	if (pArea == NULL)
	{
		pArea = new CampArea(szArea);
		m_areas.Add(pArea, szArea, 0);
	}

	Site* pSite = new Site(szSite, lat, lon);
	pArea->GetSites().Add(pSite, L"", 0);
	pArea->Print();

	pSite->Release();
	return S_OK;
}

HRESULT __stdcall CampSight::UnitTest()
{
	wchar_t rpt[1024];

	m_test->Report(rpt, 1024);
	wprintf(L"%s\n", rpt);

	std::wstring url = L"https://www.google.com/maps?q=" +
		std::to_wstring(32.251301564676666) + L"," + std::to_wstring(-97.81124046064532);

	//ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);

	CampArea* pArea = NULL;

	m_areas.GetByTag(L"Dinosaur Valley State Park", (IUnknown**)&pArea);

	Database* pDB = new Database();

	pArea->Save(*pDB);

	pDB->Release();
	pDB = new Database();

	pArea->Load(*pDB);
	pDB->Release();
	return S_OK;
}

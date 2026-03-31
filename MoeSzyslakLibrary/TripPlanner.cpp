#include "pch.h"
#include "TripPlanner.h"

Stop::Stop()
{
    m_cRef = 1;
}

Stop::~Stop()
{
}

ULONG Stop::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Stop::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Stop::QueryInterface(REFIID riid, LPVOID* ppvObj)
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



TripPlanner::TripPlanner()
{
    m_cRef = 1;
    m_pStop = NULL;
}

TripPlanner::~TripPlanner()
{
    if (m_pStop != NULL)
		m_pStop->Release();

}

ULONG TripPlanner::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall TripPlanner::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall TripPlanner::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == TripPlannerInf::m_iid)
    {
        *ppvObj = static_cast<TripPlannerInf::ITRIPPLANNER*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall TripPlanner::UnitTest()
{
	Name(L"Chicago Trip");
    wprintf(L"Trip Name:\t%s\n", m_name.c_str());

    Stop* pStop = AddStop();
	pStop->Start(L"Home");

	return S_OK;
}

Stop* TripPlanner::AddStop()
{
    if (m_pStop != NULL)
        m_pStop->Release();

	m_pStop = new Stop();
	return m_pStop;
}
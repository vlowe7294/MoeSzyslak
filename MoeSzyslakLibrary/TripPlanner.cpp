#include "pch.h"
#include "TripPlanner.h"

TripPlanner::TripPlanner()
{
    m_cRef = 1;
}

TripPlanner::~TripPlanner()
{

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
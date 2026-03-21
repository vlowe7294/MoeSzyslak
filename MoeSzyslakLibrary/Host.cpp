#include "pch.h"
#include "Host.h"
#include "VLString.h"

UINT Session::m_nNextID = 8751;

Session::Session()
{
    m_cRef = 1;
    m_nID = m_nNextID;
    m_nNextID += 12;
    m_lastAccess.Init();
}

Session::~Session()
{

}

ULONG Session::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Session::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Session::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

void Session::GetLastAccess(IUnknown** iunk)
{
    *iunk = (IUnknown*)m_lastAccess;
}

void Session::ParseCommand(LPCWSTR cmd)
{
    VLStringCollection prm, var;
    wstring lne;

    m_strInf->Set(cmd);
    prm.Split(cmd, L';');
    m_strInf.BufferSize(2000);

    while (prm.ForEach(lne) == S_OK)
    {
        
    }
}




Host::Host()
{
    m_cRef = 1;
}

Host::~Host()
{

}

ULONG Host::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Host::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Host::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == HOST_IID)
    {
        *ppvObj = static_cast<IHOST*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall Host::ParseCommand(UINT* nSession, LPCWSTR cmd)
{
    return S_OK;
}
#include "pch.h"
#include "VLDateTime.h"
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

VLDateTime::VLDateTime()
{
    std::tm now;
    m_timeSec = std::time(0);   // get time now
    m_cRef = 1;

    localtime_s(&now, &m_timeSec);

    m_data[0] = now.tm_year + 1900;
    m_data[2] = now.tm_mday;
    m_data[1] = now.tm_mon + 1;

    m_nMin = now.tm_hour * 60;
    m_nMin += now.tm_min;
    m_nSec = now.tm_sec;
}

VLDateTime::VLDateTime(VLDateTime& dt)
{
    m_cRef = 1;
    Copy(dt);
}

void VLDateTime::SetTimeStruct()
{
    tm now;
    memset(&now, 0, sizeof(tm));

    now.tm_year = m_data[0] - 1900;
    now.tm_mday = m_data[2];
    now.tm_mon = m_data[1] - 1;

    m_timeSec = mktime(&now);
    GetFromTimeStruct();
}

VLDateTime::VLDateTime(int yr, int mnth, int dy)
{
    m_cRef = 1;
    SetDay(yr, mnth, dy);
}

VLDateTime::~VLDateTime()
{

}

HRESULT __stdcall VLDateTime::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == DATETIME_IID)
    {
        *ppvObj = static_cast<IDATETIME*>(this);
        AddRef();
        return NOERROR;
    }

    else
        return E_NOINTERFACE;
}

ULONG __stdcall VLDateTime::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall VLDateTime::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
    }

    return m_cRef;
}

HRESULT __stdcall VLDateTime::SetDay(UINT yr, UINT mnth, UINT dy)
{
    m_data[0] = yr;
    m_data[1] = mnth;
    m_data[2] = dy;

    m_nSec = 0.0;
    m_nMin = 0;
    m_timeSec = 0;


    if (yr > 0)
        SetTimeStruct();

    return S_OK;

}

HRESULT __stdcall VLDateTime::SetTime(UINT hr, UINT min, UINT sec)
{
    tm t;
    localtime_s(&t, &m_timeSec);
    t.tm_hour = hr;
    t.tm_min = min;
    t.tm_sec = sec;

    m_timeSec = mktime(&t);
    GetFromTimeStruct();
    return S_OK;
}

HRESULT __stdcall VLDateTime::GetDay(UINT* yr, UINT* mnth, UINT* dy)
{
    *yr = m_data[0];
    *mnth = m_data[1];
    *dy = m_data[2];
    return S_OK;
}

HRESULT __stdcall VLDateTime::GetTime(UINT* hr, UINT* min, UINT* sec)
{
    tm t;
    localtime_s(&t, &m_timeSec);
    *hr = t.tm_hour;

    *min = t.tm_min;
    *sec = t.tm_sec;
    return S_OK;
}

HRESULT __stdcall VLDateTime::GetDifference(IUnknown* iSubAmt, int* nSec)
{
    VLDateTime* t = NULL;

    iSubAmt->QueryInterface(DATETIME_IID, (void**)&t);
    *nSec = 0;

    if (t == NULL)
        return E_FAIL;

    *nSec = (int)m_timeSec - (int)t->m_timeSec;
    return S_OK;
}

HRESULT __stdcall VLDateTime::ToString(IUnknown* iunk)
{
    StringInf iStr;
    iStr.Attach(iunk);
    int hr;

    std::wstring ret = std::to_wstring(m_data[0]) + L"-";

    if (m_data[1] < 10)
        ret += L"0";
    ret += std::to_wstring(m_data[1]) + L"-";

    if (m_data[2] < 10)
        ret += L"0";

    ret += std::to_wstring(m_data[2]);
    ret += L" ";

    hr = m_nMin / 60;

    if (hr < 10)
        ret += L"0";

    ret += std::to_wstring(hr) + L":";
    hr = m_nMin % 60;

    if (hr < 10)
        ret += L"0";

    ret += std::to_wstring(hr);
    iStr->Set(ret.c_str());
    return S_OK;
}

HRESULT __stdcall VLDateTime::TimeFormat(IUnknown* iunk)
{
    StringInf iStr;

    iStr.Attach(iunk);

    int hr = m_nMin / 60;
    std::wstring s, sec(L"AM"), ret;

    if (hr >= 12)
        sec = L"PM";

    if (hr > 12)
        hr = hr - 12;

    s = std::to_wstring(hr);

    if (s.length() < 2)
        s = L"0" + s;

    ret = s + L":";

    s = std::to_wstring(m_nMin % 60);

    if (s.length() < 2)
        s = L"0" + s;

    ret += s + L" " + sec;
    iStr->Set(ret.c_str());
    return S_OK;
}

HRESULT __stdcall VLDateTime::Copy(IUnknown* iUnk)
{
    VLDateTime* pFrm = NULL;
    iUnk->QueryInterface(DATETIME_IID, (void**)&pFrm);

    if (pFrm == NULL)
        return E_FAIL;

    memcpy(m_data, pFrm->m_data, sizeof(int) * 3);
    m_nMin = pFrm->m_nMin;
    m_nSec = pFrm->m_nSec;

    m_timeSec = pFrm->m_timeSec;
    pFrm->Release();
    return S_OK;
}

HRESULT __stdcall VLDateTime::SetToNow()
{
    VLDateTime nw;
    Copy(nw);
    return S_OK;
}

void VLDateTime::GetFromTimeStruct()
{
    tm t;
    localtime_s(&t, &m_timeSec);

    m_data[0] = t.tm_year + 1900;
    m_data[2] = t.tm_mday;
    m_data[1] = t.tm_mon + 1;

    m_nMin = t.tm_hour * 60;
    m_nMin += t.tm_min;
    m_nSec = t.tm_sec;
}

void VLDateTime::AddSeconds(double nSec)
{
    m_nSec += nSec;
    m_timeSec += (time_t)nSec;
}

void  VLDateTime::AddMinutes(int nMin)
{
    AddSeconds(60.0 * (double)nMin);
    m_nMin += nMin;
}

void VLDateTime::Parse(std::wstring inStr)
{
    std::tm t{};
    std::wistringstream ss(inStr);
    ss >> std::get_time(&t, L"%Y-%m-%d %H:%M");

    if (!ss.fail())
    {
        t.tm_isdst = -1;
        m_timeSec = mktime(&t);
        m_data[0] = t.tm_year + 1900;
        m_data[2] = t.tm_mday;
        m_data[1] = t.tm_mon + 1;

        m_nMin = t.tm_hour * 60;
        m_nMin += t.tm_min;
        m_nSec = t.tm_sec;
    }
}

void VLDateTime::AddDays(int nDays)
{
    m_data[2] += nDays;
    SetTimeStruct();

}

void VLDateTime::AddMonths(int nMonths)
{
    m_data[1] += nMonths;
    SetTimeStruct();

}

bool VLDateTime::operator > (VLDateTime& dt)
{
    return (m_timeSec > dt.m_timeSec);
}

void VLDateTime::Print()
{

}

void VLDateTime::Copy(VLDateTime& tm)
{
    memcpy(m_data, tm.m_data, sizeof(int) * 3);

    m_nMin = tm.m_nMin;
    m_nSec = tm.m_nSec;
    m_timeSec = tm.m_timeSec;
}

bool VLDateTime::Compare(VLDateTime& dt)
{
    if (memcmp(m_data, dt.m_data, sizeof(int) * 3) != 0)
        return false;

    if (m_nMin != dt.m_nMin || m_nSec != dt.m_nSec)
        return false;

    if (m_timeSec != dt.m_timeSec)
        return false;

    return true;
}
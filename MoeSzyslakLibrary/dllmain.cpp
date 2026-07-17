// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "Host.h"
#include "InterfaceCollection.h"
#include "VLDateTime.h"
#include "VLString.h"
#include "TripPlanner.h"
#include "Neverwinter\NeverwinterNights.h"
#include "Testing.h"
#include "IOS\IOS.h"
#include "User.h"
#include "Finance.h"
#include "AstralWorkshop\AstralWorkshop.h"
#include "CampSight.h"



HMODULE JimboJonesLibrary::m_hModule = NULL;
JimboJonesLibrary::PROC_GETLIBRARYVERSION JimboJonesLibrary::m_pGetLibraryVersion = NULL;
JimboJonesLibrary::PROC_CREATEINTERFACE JimboJonesLibrary::m_pCreateInterface = NULL;

void JimboJonesLibrary::Init()
{
    if (m_hModule == NULL)
    {
        m_hModule = LoadLibrary(L"JimboJonesLibrary.dll");
        m_pGetLibraryVersion = (PROC_GETLIBRARYVERSION)GetProcAddress(m_hModule, "GetLibraryVersion");
        m_pCreateInterface = (PROC_CREATEINTERFACE)GetProcAddress(m_hModule, "CreateJimboJonesInterface");
    }

}

UINT JimboJonesLibrary::GetJimboLibraryVersion()
{
    Init();

    if (m_pGetLibraryVersion == NULL)
        return 0;

    return m_pGetLibraryVersion();

}

IUnknown* JimboJonesLibrary::CreateInterface(UINT nClassID)
{
    IUnknown* iunk = NULL;
    
    Init();

    m_pCreateInterface(nClassID, &iunk);
    return iunk;
}

JimboJonesLibrary g_jimboLib;

#define VERSION 1643

VLString g_dllPath;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_dllPath.SetBufferLen(MAX_PATH);
        GetModuleFileNameW(hModule, g_dllPath, MAX_PATH);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#define UI_EXPORT __declspec(dllexport)

UI_EXPORT UINT __stdcall GetLibraryVersion()
{
    return VERSION;
}

wstring VersionAsString()
{
    wstring s;

    UINT v = VERSION;

#ifdef _DEBUG
    s = L"Debug Version 1.3.6." + std::to_wstring(v - 1530);
#else
    s = L"Release Version 1.3.6." + std::to_wstring(v - 1530);
#endif
    return s;
}

UI_EXPORT void _cdecl CreateMoeSzyslakInterface(UINT nClassID, IUnknown** iunk)
{
    *iunk = NULL;

    switch (nClassID)
    {

    case VariableInf::ClassID:
        *iunk = (IUnknown*)new VLVariable();
        break;

    case CLASSID::ASTRALWORKSHOP:
        *iunk = new AstralWorkshop();
        break;

    case CLASSID::INTERFACELIST:
        *iunk = (IUnknown*)new InterfaceCollection();
        break;   

    case CommsInf::ClassID:
        *iunk = (IUnknown*)new CommGeneralPage();
        break;

    case CLASSID::USERSERVICE:
        *iunk = (IUnknown*)new UserService();
        break;

    case UserInf::ClassID:
        *iunk = (IUnknown*)new User(L"New User", L"guest123", FALSE);
        break;

    case StringInf::ClassID:
        *iunk = (IUnknown*)new VLString();
        break;

    case FinanceInf::ClassID:
        *iunk = (IUnknown*)new Finance();
        break;

    case TestingInf::ClassID:
        *iunk = (IUnknown*)new Testing();
        break;

    case CLASSID::DATETIME:
        *iunk = (IUnknown*)new VLDateTime();
        break;

    case CLASSID::HOST:
        *iunk = (IUnknown*)new Host();
        break;

    case CLASSID::TRIPPLANNER:
        *iunk = (IUnknown*)new TripPlanner();
        break;

    case NeverwinterInf::ClassID:
        *iunk = (IUnknown*)new NeverwinterNights();
        break;

    case CLASSID::DATABASE:
        *iunk = (IDATABASE*)new Database();
        break;

    case IOSInf::ClassID:
        *iunk = (IUnknown*)new IOS();
        break; 

    case CLASSID::CAMPSIGHT:
        *iunk = (IUnknown*)new CampSight();
        break;
    }
}

UI_EXPORT void _cdecl FreeMoeSzyslakInterface(IUnknown* iunk)
{
    if (iunk != NULL)
		iunk->Release();
}


InterfaceCollectionInf g_iObjects;

UI_EXPORT UINT _cdecl CreateMoeSzyslakHandle(UINT nClassID)
{
    IUnknown* iunk = NULL;
    int nCnt = 0;
    CreateMoeSzyslakInterface(nClassID, &iunk);

    if (iunk == NULL)
        return nCnt;

    g_iObjects->Add(iunk, L"", nClassID);
    iunk->Release();
    g_iObjects->Count(&nCnt);
    return nCnt;
}

UI_EXPORT UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, const wchar_t* szCmd)
{
    hObj--;
    wstring cmd = std::to_wstring(hObj) + L" ";	

    cmd += szCmd;
    if (FAILED(g_iObjects->Command(cmd.c_str())))
        return 1;
    else
        return 0;
}

UI_EXPORT void  _cdecl DestroyMoeSzyslakHandle(UINT hObj)
{
    hObj--;
    g_iObjects->Remove(hObj);
}

UI_EXPORT void _cdecl MoeSzyslakGetReturnString(UINT hObj, wchar_t* szRet, UINT len)
{
    StringInf iRetStr;
    g_iObjects->GetReturnString(iRetStr);
    iRetStr.BufferSize(len);
    wcsncpy_s(szRet, len, (const wchar_t*)iRetStr, len - 5);
    iRetStr->Set(L"");
}

const wchar_t* DEFAULT_HTML_TEMPLATE = LR"(
<!DOCTYPE html>
<html>
<head>
    <title>Moe Szyslak Library</title>
</head> 
<body>
</body>
</html>
)";

UI_EXPORT void _cdecl MoeSzyslakHTML(UINT nClassID, const wchar_t* szGet, wchar_t* szRet, UINT len)
{
    static StringInf iRetStr;
    static FinanceInf iFnce;

    if ((IUnknown*)iRetStr == NULL)
        iRetStr.Init();

    iRetStr.BufferSize(len);

    if (nClassID == FinanceInf::ClassID)
    {
        if ((IUnknown*)iFnce == NULL)
            iFnce.Attach();

        iFnce->HTMLPage(szGet, iRetStr);
        wcsncpy_s(szRet, len, (const wchar_t*)iRetStr, len - 5);
        return;
    }

    wstring htm = DEFAULT_HTML_TEMPLATE, str;

    

    int i = htm.find(L"</body>");

    if (i > 0)
    {
        str = htm.substr(0, i);
        str += L"<div>Moe Szyslak Library " + VersionAsString();

        str += L"</div>";
        str += htm.substr(i);
        htm = str;
    }

    

    
    iRetStr.Set(htm.c_str());
    wcsncpy_s(szRet, len, (const wchar_t*)iRetStr, len - 5);
}

const wchar_t* GetLibraryPath()
{
    return g_dllPath;
}

UINT AddHandle(IUnknown* iunk, UINT nClassID)
{
    int nCnt = 0;
    g_iObjects->Count(&nCnt);
    g_iObjects->Add(iunk, L"", nClassID);
    nCnt++;
    return nCnt;
}

IUnknown* GetInterfaceByHandle(UINT hObj)
{
    hObj--;
    IUnknown* iunk = NULL;

    g_iObjects->Get(hObj, &iunk);

    return iunk;

}
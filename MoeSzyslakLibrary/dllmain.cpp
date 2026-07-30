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

#define VERSION 1657

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

    case CLASSID::VARIABLECOLLECTION:
        *iunk = (IUnknown*)new VariableCollection();
        break;
    }
}

UI_EXPORT void _cdecl FreeMoeSzyslakInterface(IUnknown* iunk)
{
    if (iunk != NULL)
		iunk->Release();
}


ComCollection g_Objects;

UI_EXPORT UINT _cdecl CreateMoeSzyslakHandle(UINT nClassID)
{
    IUnknown* iunk = NULL;
    int nCnt = 0;
    CreateMoeSzyslakInterface(nClassID, &iunk);

    if (iunk == NULL)
        return nCnt;

    g_Objects.Add(iunk, L"", nClassID);
    iunk->Release();
    nCnt = g_Objects.Count();
    return nCnt;
}

UI_EXPORT UINT _cdecl InvokeMoeSzyslakHandle(UINT hObj, UINT hVariables)
{
    hObj--;
    hVariables--;
    IUnknown* iunk = g_Objects.Get(hObj), *iVariableList = g_Objects.Get(hVariables);
    IMOEDISPATCH* iDisp;

    if (iunk == NULL)
        return -1;

    iunk->QueryInterface(DISPATCH_IID, (void**)&iDisp);
    iunk->Release();

    if (iDisp == NULL || iVariableList == NULL)
    {
        if (iDisp != NULL)
            iDisp->Release();

        if (iVariableList != NULL)
            iVariableList->Release();

        iunk->Release();
        return -1;
    }

    iDisp->Dispatch(iVariableList);

    iDisp->Release();
    iVariableList->Release();
    return 0;
}

UI_EXPORT void _cdecl MoeSzyslakSetInt(UINT hObj, const wchar_t* szVarName, int nVal)
{
    hObj--;
    IUnknown* iunk = g_Objects.Get(hObj);
    IVARIABLELIST* iVars = NULL;
    hObj--;

    if (iunk == NULL)
        return;

    iunk->QueryInterface(VARIABLELIST_IID, (void**)&iVars);
    iunk->Release();

    if (iVars == NULL)
        return;

    iVars->SetAsInt(szVarName, nVal, 0);
}

UI_EXPORT void  _cdecl DestroyMoeSzyslakHandle(UINT hObj)
{
    hObj--;
    g_Objects.Remove(hObj);
}


UI_EXPORT void _cdecl MoeSzyslakHTML(UINT nClassID, const wchar_t* szGet, wchar_t* szRet, UINT len)
{
    
}

const wchar_t* GetLibraryPath()
{
    return g_dllPath;
}

UINT AddHandle(IUnknown* iunk, UINT nClassID)
{
    int nCnt = g_Objects.Count();
    g_Objects.Add(iunk, L"", nClassID);
    nCnt++;
    return nCnt;
}

IUnknown* GetInterfaceByHandle(UINT hObj)
{
    hObj--;
    IUnknown* iunk = g_Objects.Get(hObj);

    return iunk;

}
#include "pch.h"
#include "User.h"
#include "Testing.h"


InterfaceCollection User::m_masterUserList;
BOOL User::m_bInit = FALSE;

void User::InitMasterList()
{
    User* iNewUsr;
    VariableInfCollection ivars;

    if (m_bInit == FALSE)
    {
        m_bInit = TRUE;

        iNewUsr = new User(true, L"Vaughn", L"ZFyZH8DuKemv");
        iNewUsr->Properties(ivars);

        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        iNewUsr->m_pIsAdmin->SetAsBool(TRUE);
        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        m_masterUserList.Add(iNewUsr, L"Vaughn", UserInf::ClassID);
        iNewUsr->Release();
    }
}

static int instanceCounter = 0;

User::User(bool bAdmin, const wchar_t* szName, const wchar_t* szPwd)
{
    instanceCounter++;
    m_instance = instanceCounter;
    m_cRef = 1;
    g_memoryChecker.IncrementInstance(UserInf::ClassID);

    m_name = szName;
    m_pProperties = new VariableCollection();

    m_password = szPwd;
    m_pIsAdmin = m_pProperties->NewVariable(L"Is Administrator");

	if (bAdmin)  // can't imagine a scenario where we would want to create a non-admin user with this constructor, but just in case...
        m_pIsAdmin->SetAsBool(TRUE);
	else
        m_pIsAdmin->SetAsBool(FALSE);

    m_pIsAdmin->SetLocked(TRUE);
    m_bIsLoggedIn = FALSE;

    m_pEmail = m_pProperties->NewVariable(L"Email");
    m_iLastActive.Init();
}


User::User(const wchar_t* szName, const wchar_t* szPwd)
{
    instanceCounter++;
    m_instance = instanceCounter;

    m_cRef = 1;
    g_memoryChecker.IncrementInstance(UserInf::ClassID);
    InitMasterList();

	m_pProperties = new VariableCollection();

    m_name = szName;
    m_password = szPwd;

    m_pIsAdmin = m_pProperties->NewVariable(L"Is Administrator");
    m_pIsAdmin->SetAsBool(FALSE);
    m_pIsAdmin->SetLocked(TRUE);

    m_bIsLoggedIn = FALSE;
    m_pEmail = m_pProperties->NewVariable(L"Email");
    m_iLastActive.Init();
}

User::~User()
{
	m_pProperties->Release();
    g_memoryChecker.DecrementInstance(UserInf::ClassID);
}

ULONG User::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall User::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall User::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == USER_IID)
    {
        *ppvObj = static_cast<IUSER*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall User::Command(const wchar_t* szCmd)
{
    HRESULT hr = E_FAIL;
    VLStringCollection cmds;
    MoeInf<IDATETIME, CLASSID::DATETIME> dtNow;
    
    if (!szCmd || wcslen(szCmd) == 0)
        return E_INVALIDARG;

    dtNow.Init();
    int nSec = 0;
	dtNow->GetDifference(m_iLastActive, &nSec);  

    if (nSec > 900)// users that have been idle for more than 15 minutes will be logged out
    {
        m_bIsLoggedIn = FALSE;
    }

    m_iLastActive->SetToNow();
    cmds.Split(szCmd, L' ');
    cmds.ToLower(0);

    switch (szCmd[0])
    {
    default:
        hr = m_pProperties->Command(szCmd);
    }

    return hr;
}

HRESULT __stdcall User::GetReturnString(IUnknown** iStr)
{
    return m_pProperties->GetReturnString(iStr);
}

HRESULT __stdcall User::UnitTest()
{
	IUnknown* iunk = NULL;
	IUSERSERVICE* iUserSvc = NULL;

    CreateMoeSzyslakInterface(CLASSID::USERSERVICE, &iunk);
    iunk->QueryInterface(USERSERVICE_IID, (void**)&iUserSvc);
    HRESULT hr = iUserSvc->UnitTest();

	iUserSvc->Release();
    iunk->Release();

    return hr;
}

HRESULT __stdcall User::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
    return S_OK;
}

HRESULT __stdcall User::Login(const wchar_t* szName, const wchar_t* szPwd)
{
    if (m_name == szName && m_password == szPwd)
    {
        m_iLastActive->SetToNow();
        m_bIsLoggedIn = TRUE;
    }

    return S_OK;
}

HRESULT __stdcall User::SaveUserList(IUnknown* iunk)
{
    Table* iTbl = NULL;
    UserInf iUsr;
    VariableInfCollection iprp;
    MoeInf<IDATABASE, CLASSID::DATABASE> iDB;

    int nCnt = 0;
	iDB.Attach(iunk);

    iDB->GetTable(L"users", (IUnknown**)&iTbl); 
	iTbl->Clear();

    while (m_masterUserList.ForEach(iUsr) == S_OK)
    {
        if (nCnt > 0)
            iTbl->NewRow();

        iUsr->Properties(iprp);
        iTbl->Set(L"Login Name", iprp.Get(L"Login Name").c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
        iTbl->Set(L"Password", iprp.Get(L"Password").c_str(), VLVariable::VAR_TYPE::TYPE_STRING);

        iTbl->Set(L"Is Administrator", iprp.Get(L"Is Administrator").c_str(), VLVariable::VAR_TYPE::TYPE_BOOL);
        iTbl->Set(L"Email", iprp.Get(L"Email").c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
        nCnt++;
    }

	return S_OK;
}

HRESULT __stdcall User::AddToMasterList(const wchar_t* szTag, const wchar_t* szPassword)
{
	UserInf iUsr;
    VariableInfCollection iprp;
	BOOL bIsAdmin = FALSE;
	IUnknown* iunk = NULL;

	if (m_bIsLoggedIn == FALSE)  // have to be logged in to add users to the master list
        return E_FAIL;

    m_pIsAdmin->GetAsBool(&bIsAdmin);

    if (bIsAdmin == FALSE)  // only administrators can add users to the master list
        return E_FAIL;
    
    m_masterUserList.GetByTag(szTag, &iunk);

    if (iunk == NULL)
    {
        iUsr.Init();
        iUsr->Properties(iprp);
        iprp.Set(L"Login Name", szTag);

        iprp.Set(L"Password", L"password");
        m_masterUserList.Add(iUsr, szTag, UserInf::ClassID);
		return S_OK;
    }
	else  // user already exists
        return E_FAIL;
}

HRESULT __stdcall User::LoadUserList(IUnknown* iunk)
{
    // Table* iTbl = NULL;
    User* pUsr;

    UINT nCnt = 0;
    int i = 0;
	StringInf iStr;
    wstring nme;

    MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
    MoeInf<ITABLE, CLASSID::TABLE> iTbl;
    iDB.Attach(iunk);
    iDB->GetTable(L"users", (IUnknown**)iTbl);
	iStr.Init();

    iTbl->GoToTopRow();
    iTbl->RowCount(&nCnt);

    if (nCnt == 0)
        return S_OK;

    m_masterUserList.Clear();

    while (nCnt > 0)
    {
        iTbl->Get(L"Login Name", iStr);
		nme = (const wchar_t*)iStr;
        iTbl->Get(L"Password", iStr);

        pUsr = new User(nme.c_str(), iStr);
        iTbl->Get(L"Is Administrator", iStr);

        pUsr->m_pIsAdmin->SetLocked(FALSE);
        pUsr->m_pIsAdmin->SetString(iStr);
        pUsr->m_pIsAdmin->SetLocked(TRUE);

        iTbl->Get(L"Email", iStr);
        pUsr->m_pEmail->SetString(iStr);
        m_masterUserList.Add(pUsr, pUsr->m_name.c_str(), UserInf::ClassID);
        nCnt--;

		Table* pTbl = (Table*)((IUnknown*)iTbl);
        pTbl->NextRow();
        pUsr->Release();
    }

    return S_OK;
}

HRESULT __stdcall User::Copy(IUnknown* iunk)
{
    BOOL b = FALSE;
    UserInf iCopyFrom;
    VariableInfCollection iVar;

    iCopyFrom.Attach(iunk);
    iCopyFrom->Properties(iVar);

    m_password = iVar.Get(L"Password");

    m_pIsAdmin->SetLocked(FALSE);

    if (iVar.GetBool(L"Is Administrator"))
        m_pIsAdmin->SetAsBool(TRUE);
    else
        m_pIsAdmin->SetAsBool(FALSE);

    m_pIsAdmin->SetLocked(TRUE);

    m_pEmail->SetString(iVar.Get(L"Email").c_str());
    return S_OK;
}



bool User::Compare(User& usr)
{
    if (m_name != usr.m_name)
        return false;

    if (m_password != usr.m_name)
        return false;    

    return true;
}




void User::EditMasterList(User& usr)
{
    BOOL bIsAdmin = FALSE;
    UserInf iuser;

    m_pIsAdmin->GetAsBool(&bIsAdmin);

    if (bIsAdmin == FALSE || m_bIsLoggedIn == FALSE)
        // you have to be logged in and an admin before you can make any changes to the master list
        return;

    m_masterUserList.GetByTag(usr.m_name.c_str(), iuser);

    if ((IUnknown*)iuser != NULL)
    {
        iuser->Copy(&usr);

    }
}

BOOL User::IsLoggedIn()
{
    MoeInf<IDATETIME, CLASSID::DATETIME> dtNow;
    dtNow.Init();

    int nSec = 0;
    dtNow->GetDifference(m_iLastActive, &nSec);

    if (nSec > 900)// users that have been idle for more than 15 minutes will be logged out
    {
        m_bIsLoggedIn = FALSE;
    }
    else
        m_iLastActive->SetToNow();

    return m_bIsLoggedIn;

}




UserService::UserService()
{
    m_cRef = 1;
}

UserService::~UserService()
{
    printf("ComObjectBase destructor called\n");

}

ULONG UserService::AddRef()
{
    m_cRef++;

	ULONG newCount = m_cRef;
    printf("AddRef -> %lu\n", newCount);
    
    return m_cRef;
}

ULONG __stdcall UserService::Release()
{
    m_cRef--;

    ULONG newCount = m_cRef;

    printf("Release -> %lu\n", newCount);

    if (newCount == 0)
    {
        printf("Refcount reached zero, deleting object\n");
        delete this;
    }

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall UserService::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == USERSERVICE_IID)
    {
        *ppvObj = static_cast<IUSERSERVICE*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall UserService::GetUser(const wchar_t* szName, IUnknown** iUsr)
{
    int nUser = 0;
	IUnknown* iunk = NULL;
    m_users.Count(&nUser);

    if (nUser == 0)
        LoadUsers();

    m_users.GetByTag(szName, &iunk);

    if (iunk != NULL)
		iunk->AddRef();

    *iUsr = iunk;
    return S_OK;
}

HRESULT UserService::UnitTest()
{
    User* iUsr = NULL;

	GetUser(L"Vaughn", (IUnknown**)&iUsr);

    iUsr->Login(L"Vaughn", L"ZFyZH8DuKemv");
    iUsr->IsLoggedIn();	
	iUsr->Release();

    m_users.GetByTag(L"Nevin", (IUnknown**)&iUsr);

    if (iUsr == NULL)
    {
        iUsr = new User(L"Nevin", L"guest123");
        m_users.Add(iUsr, L"Nevin", UserInf::ClassID);
        iUsr->Release();
    }
    

    return S_OK;
}

void UserService::LoadUsers()
{
	m_users.Clear();

    User* iUsr = new User(L"Vaughn", L"ZFyZH8DuKemv");
    m_users.Add(iUsr, L"Vaughn", UserInf::ClassID);
    iUsr->Release();

    iUsr = new User(L"guest", L"guest123");
    m_users.Add(iUsr, L"guest", UserInf::ClassID);
    iUsr->Release();
    iUsr = NULL;
}


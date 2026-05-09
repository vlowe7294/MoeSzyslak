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

        iNewUsr = new User(true);
        iNewUsr->Properties(ivars);

        ivars.Set(L"Login Name", L"Vaughn");
        ivars.Set(L"Password", L"ZFyZH8DuKemv");

        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        iNewUsr->m_pIsAdmin->SetAsBool(TRUE);
        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        m_masterUserList.Add(iNewUsr, L"Vaughn", UserInf::ClassID);
        iNewUsr->Release();
    }
}

static int instanceCounter = 0;

User::User(bool bAdmin)
{
    instanceCounter++;
    m_instance = instanceCounter;
    m_cRef = 1;
    g_memoryChecker.IncrementInstance(UserInf::ClassID);
    m_pProperties = new VariableCollection();

    m_pName = m_pProperties->NewVariable(L"Login Name");
    m_pName->SetString(L"New User");
    m_pName->CanBeEmpty(false);

    m_pPassword = m_pProperties->NewVariable(L"Password");
    m_pPassword->SetString(L"xxxxx");
    m_pPassword->CanBeEmpty(false);

    m_pIsAdmin = m_pProperties->NewVariable(L"Is Administrator");

	if (bAdmin)  // can't imagine a scenario where we would want to create a non-admin user with this constructor, but just in case...
        m_pIsAdmin->SetAsBool(TRUE);
	else
        m_pIsAdmin->SetAsBool(FALSE);

    m_pIsAdmin->SetLocked(TRUE);

    m_pIsLoggedIn = m_pProperties->NewVariable(L"Is Logged In");
    m_pIsLoggedIn->SetAsBool(FALSE);
    m_pIsLoggedIn->SetLocked(TRUE);

    m_pEmail = m_pProperties->NewVariable(L"Email");
    m_iLastActive.Init();
}


User::User()
{
    instanceCounter++;
    m_instance = instanceCounter;

    m_cRef = 1;
    g_memoryChecker.IncrementInstance(UserInf::ClassID);
    InitMasterList();

	m_pProperties = new VariableCollection();

    m_pName = m_pProperties->NewVariable(L"Login Name");
    m_pName->SetString(L"New User");
    m_pName->CanBeEmpty(false);   

    m_pPassword = m_pProperties->NewVariable(L"Password");
    m_pPassword->SetString(L"xxxxx");
    m_pPassword->CanBeEmpty(false);

    m_pIsAdmin = m_pProperties->NewVariable(L"Is Administrator");
    m_pIsAdmin->SetAsBool(FALSE);
    m_pIsAdmin->SetLocked(TRUE);

    m_pIsLoggedIn = m_pProperties->NewVariable(L"Is Logged In");
    m_pIsLoggedIn->SetAsBool(FALSE);
    m_pIsLoggedIn->SetLocked(TRUE); 

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
        m_pIsLoggedIn->SetLocked(FALSE);
        m_pIsLoggedIn->SetAsBool(FALSE);
        m_pIsLoggedIn->SetLocked(TRUE);
    }

    m_iLastActive->SetToNow();
    cmds.Split(szCmd, L' ');
    cmds.ToLower(0);

    switch (szCmd[0])
    {
    case L'l':
    case L'L':
        if (cmds.Compare(0, L"login"))
            hr = Login();
        break;

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
    /*TODO: 
    add so that if password is default, forces user to change.
    add a "guest" user
    */

    User* pUser = new User();
    VariableInfCollection iPrp;
    pUser->Properties(iPrp);

    iPrp.Set(L"Login Name", L"Nevin");
    iPrp.Set(L"Password", L"ZFyZH8DuKemv");
    
    EditMasterList(*pUser);
    pUser->Release();
    

    VLDateTime* pTime = (VLDateTime*)((IUnknown*)m_iLastActive);
	pTime->AddMinutes(-45);  // checking to see if user will be logged out after 15 minutes of inactivity       
    return S_OK;
}

HRESULT __stdcall User::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
    return S_OK;
}

HRESULT __stdcall User::Login()
{
    User* iUsr = NULL;
    BOOL b = FALSE;

    m_masterUserList.GetByTag(m_pName->GetString().c_str(), (IUnknown**)&iUsr);

    if (iUsr == NULL)
        return S_OK;

    if (Compare(*iUsr))
    {
		iUsr->m_pIsAdmin->GetAsBool(&b);
        m_pIsAdmin->SetLocked(FALSE);
        m_pIsAdmin->SetAsBool(b);
        m_pIsAdmin->SetLocked(TRUE);
        
        m_pIsLoggedIn->SetLocked(FALSE);
        m_pIsLoggedIn->SetAsBool(TRUE);
        m_pIsLoggedIn->SetLocked(TRUE);        
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

	m_pIsLoggedIn->GetAsBool(&bIsAdmin);

	if (bIsAdmin == FALSE)  // have to be logged in to add users to the master list
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
        pUsr = new User();
        iTbl->Get(L"Login Name", iStr);
        pUsr->m_pName->SetString(iStr);

        iTbl->Get(L"Password", iStr);
        pUsr->m_pPassword->SetString(iStr);

        iTbl->Get(L"Is Administrator", iStr);
        pUsr->m_pIsAdmin->SetLocked(FALSE);
        pUsr->m_pIsAdmin->SetString(iStr);
        pUsr->m_pIsAdmin->SetLocked(TRUE);

        iTbl->Get(L"Email", iStr);
        pUsr->m_pEmail->SetString(iStr);
        m_masterUserList.Add(pUsr, pUsr->m_pName->GetString().c_str(), UserInf::ClassID);
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

    m_pPassword->SetString(iVar.Get(L"Password").c_str());

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
    UserInf iUser;
    VariableInfCollection iprp;

    usr.Properties(iprp);

    if (m_pName->GetString() != iprp.Get(L"Login Name"))
        return false;

    if (m_pPassword->GetString() != iprp.Get(L"Password"))
        return false;    

    return true;
}




void User::EditMasterList(User& usr)
{
    BOOL bIsLoggedIn = FALSE, bIsAdmin = FALSE;
    UserInf iuser;

    m_pIsLoggedIn->GetAsBool(&bIsLoggedIn);
    m_pIsAdmin->GetAsBool(&bIsAdmin);

    if (bIsAdmin == FALSE || bIsLoggedIn == FALSE)
        // you have to be logged in and an admin before you can make any changes to the master list
        return;

    m_masterUserList.GetByTag(usr.m_pName->GetString().c_str(), iuser);

    if ((IUnknown*)iuser != NULL)
    {
        iuser->Copy(&usr);

    }

}

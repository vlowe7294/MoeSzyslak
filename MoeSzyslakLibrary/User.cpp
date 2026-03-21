#include "pch.h"
#include "User.h"


InterfaceCollection User::m_masterUserList;
BOOL g_bInit = FALSE;

User::User()
{
    m_cRef = 1;
    User* iNewUsr;
    VariableInfCollection ivars;
    
    if (g_bInit == FALSE)
    {
        g_bInit = TRUE;

        iNewUsr = new User();
        iNewUsr->Properties(ivars);

        ivars.Set(L"Login Name", L"Vaughn");
        ivars.Set(L"Password", L"ZFyZH8DuKemv");

        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        iNewUsr->m_pIsAdmin->SetAsBool(TRUE);
        iNewUsr->m_pIsAdmin->SetLocked(FALSE);
        m_masterUserList.Add(iNewUsr, L"Vaughn", UserInf::ClassID);
        iNewUsr->Release();

    }


	m_pProperties = new VariableCollection();

    m_pName = m_pProperties->NewVariable(L"Login Name");
    m_pName->SetString(L"New User");
    m_pName->CanBeEmpty(false);   

    m_pPassword = m_pProperties->NewVariable(L"Password");
    m_pName->SetString(L"xxxxx");
    m_pPassword->CanBeEmpty(false);

    m_pIsAdmin = m_pProperties->NewVariable(L"Is Administrator");
    m_pIsAdmin->SetAsBool(FALSE);
    m_pIsAdmin->SetLocked(TRUE);

    m_pIsLoggedIn = m_pProperties->NewVariable(L"Is Logged In");
    m_pIsLoggedIn->SetAsBool(FALSE);
    m_pIsLoggedIn->SetLocked(TRUE);

    m_iReturnStr.Init();   
}

User::~User()
{
	m_pProperties->Release();
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
    VLStringCollection wrds;
    HRESULT hr = S_OK;
    wstring prmpt;

    wrds.Split(szCmd, L' ');
    wrds.ToLower(0);

    m_iReturnStr->Append(L"Unknown user command - ");
    m_iReturnStr->Append(wrds.Get(0).c_str());
    m_iReturnStr->Append(L"\n");
    
    return hr;
}

HRESULT __stdcall User::GetReturnString(IUnknown** iStr)
{
    *iStr = (IUnknown*)m_iReturnStr;
    return S_OK;
}

HRESULT __stdcall User::UnitTest()
{
    Database* pDB = new Database();

    

    Login();

   

    SaveUserList(*pDB);
    pDB->Save(L"MoeDatabase.db");

    pDB->Release();
    
    return S_OK;
}

HRESULT __stdcall User::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
    return S_OK;
}

void User::Login()
{
    User* iUsr = NULL;
    BOOL b = FALSE;

    m_masterUserList.GetByTag(m_pName->GetString().c_str(), (IUnknown**)&iUsr);

    if (iUsr == NULL)
        return;

    if (Compare(*iUsr))
    {
        m_pIsLoggedIn->SetLocked(FALSE);
        m_pIsLoggedIn->SetAsBool(TRUE);
        m_pIsLoggedIn->SetLocked(TRUE);
        m_iLastLogin.Init();
    }
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

    if (iprp.GetBool(L"Is Administrator"))
    {
        m_pIsAdmin->SetLocked(FALSE);
        m_pIsAdmin->SetAsBool(TRUE);
        m_pIsAdmin->SetLocked(TRUE);
    }

    return true;
}

void User::SaveUserList(Database& db)
{
    MoeInf<ITABLE, CLASSID::TABLE> iTbl;
    User* pUsr;
    
    IUnknown* iUnk = NULL;
    int nCnt = 0;

    db.GetTable(L"users", iTbl);

    while (m_masterUserList.ForEach(&iUnk) == S_OK)
    {
        if (nCnt > 0)
            iTbl->NewRow();

        pUsr = (User*)iUnk;
        iUnk = NULL;
        pUsr->Properties(&iUnk);

        VariableInfCollection iprp(iUnk);

        iTbl->Set(L"Login Name", iprp.Get(L"Login Name").c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
        iTbl->Set(L"Password", iprp.Get(L"Password").c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
        iTbl->Set(L"Is Administrator", iprp.Get(L"Is Administrator").c_str(), VLVariable::VAR_TYPE::TYPE_BOOL);
        nCnt++;
    }    
}
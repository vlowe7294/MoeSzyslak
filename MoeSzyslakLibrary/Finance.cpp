#include "pch.h"
#include "Finance.h"
#include "Testing.h"

std::atomic<int> Transaction::m_nNextID{ 1 };   

Transaction::Transaction(VLDateTime& dte, wstring ctgry, wstring desc, FREQUENCY nFreq)
{
    m_cRef = 1;
    m_pProperties = new VariableCollection();
    m_pAmount = m_pProperties->NewVariable(L"amount");
    m_pAmount->SetAsFloat(0, 0.0);


    m_id = m_nNextID.fetch_add(1, std::memory_order_relaxed);   
    m_date.Copy(dte);
    m_category = ctgry;
    m_desc = desc;
    m_nFrequency = nFreq;
}

Transaction::~Transaction()
{
    m_pProperties->Release();
}

ULONG Transaction::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Transaction::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Transaction::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

HRESULT Transaction::Command(const wchar_t* szCmd)
{
    return E_FAIL;
}

void Transaction::Print()
{
    StringInf istr;
    wprintf(L"Transaction id:  %d\n", m_id);
    istr.Init();

    m_date.ToString(istr);
    wprintf(L"date:  %s\n", (const wchar_t*)istr);
    wprintf(L"Category:  %s\n", m_category.c_str());
    wprintf(L"Description:  %s\n", m_desc.c_str());
    wprintf(L"Amount:  %s\n", m_pAmount->GetString().c_str());
}

void  Transaction::Update()
{
    VLDateTime now;
    
    while (now > m_date)
    {
        switch (m_nFrequency)
        {
        case WEEK:
            m_date.AddDays(7);
            break;

        case BIWEEK:
            m_date.AddDays(14);
            break;

        case MONTH:
            m_date.AddMonths(1);
            break;

        case YEAR:
            m_date.AddMonths(12);
            break;

        default:
            m_date.AddMonths(12);
            break;
        }
    }

}


VLDateTime g_now;

Account::Account(double bal)
{
    StringInf iStr;
    m_cRef = 1;
    m_pProperties = new VariableCollection();


    m_iAccountID = new VLVariable();
    m_iAccountID->GetDisplayName(iStr);
    iStr.Set(L"accountID");
    
    m_iOpeningBalance = m_pProperties->NewVariable(L"balance");
    m_iOpeningBalance->SetAsFloat(0, bal);

    m_iName = m_pProperties->NewVariable(L"name");
    m_iName->SetString(L"New Account");
    m_iName->CanBeEmpty(false);
}

Account::~Account()
{
    m_iAccountID->Release();
    m_pProperties->Release();
}

ULONG Account::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Account::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Account::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

HRESULT Account::Command(const wchar_t* szCmd)
{
    HRESULT hr = E_FAIL;
    VLStringCollection cmds;
    wstring s;
    int nCount = 0;

    if (!szCmd || wcslen(szCmd) == 0)
        return E_INVALIDARG;

    cmds.Split(szCmd, L' ');
    cmds.ToLower(0);
    cmds.ToLower(1);

    switch (szCmd[0])
    {
    case L'a':
    case L'A':
        if (cmds.Compare(0, L"addtransaction"))
        {
            hr = AddTransaction();
        }
        break;

    case L't':
    case L'T':
        if (cmds.Compare(0, L"transaction"))
        {
            if (cmds.Compare(1, L"count"))
            {
                hr = m_iTransactions->Count(&nCount);
                m_iRetStr.Set(std::to_wstring(nCount).c_str());
            }
        }
        break;

    default:        
        break;
    }

    return hr;
    
}

HRESULT Account::GetReturnString(IUnknown** iStr)
{
    *iStr = (IUnknown*)m_iRetStr;
    return S_OK;
}

HRESULT Account::GetTransaction(UINT ndx, IUnknown** iTran)
{
    return m_iTransactions->Get(ndx, iTran);
}

HRESULT Account::AddTransaction()
{
    VLDateTime tranDate(2026, 3, 17);
    Transaction* pNewTran = new Transaction(tranDate, L"Auto Fuel", L"Gas", Transaction::MONTH);
    m_iTransactions->Add(pNewTran, L"", 0);
    pNewTran->Release();
    return S_OK;
}

void Account::ID(IUnknown** iVar)
{
    m_iAccountID->QueryInterface(IID_IUnknown, (void**)iVar);
    m_iAccountID->Release();
}

void Account::HTMLPage(VLStringCollection& varList, wstring& inHTML)
{
    wstring str;
    VLStringCollection var;

    while (varList.ForEach(str))
    {
        var.Split(str.c_str(), L'=');

        if (var.Compare(0, L"accountID"))
            m_iAccountID->SetString(var.Get(1).c_str());
        else if (var.Compare(0, L"accountName"))
            m_iName->SetString(var.Get(1).c_str());
        else if (var.Compare(0, L"openingBalance"))
            m_iOpeningBalance->SetString(var.Get(1).c_str());
    }

    int i = inHTML.find(L"id=\"accountID\"");

    if (i > 0)
    {
        str = inHTML.substr(0, i);
        str += L" value=\"" + m_iAccountID->GetString() + L"\" ";
        str += inHTML.substr(i);
        inHTML = str;
    }

    i = inHTML.find(L"id=\"accountName\"");

    if (i > 0)
    {
        str = inHTML.substr(0, i);
        str += L" value=\"" + m_iName->GetString() + L"\" ";
        str += inHTML.substr(i);
        inHTML = str;
    }

    i = inHTML.find(L"id=\"openingBalance\"");

    if (i > 0)
    {
        str = inHTML.substr(0, i);
        str += L" value=\"" + m_iOpeningBalance->GetString() + L"\" ";
        str += inHTML.substr(i);
        inHTML = str;
    }

    
}

void Account::Update()
{
    Transaction* iTran = NULL;

    while (m_iTransactions.ForEach((IUnknown**)&iTran))
    {
        iTran->Update();
    }

}



const wchar_t* Finance::HTML_TEMPLATE = LR"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Finance Budgeting</title>

    <style>
        body {
            background-color: #1b1b2f;
            color: #eaeaea;
            font-family: "Trebuchet MS", sans-serif;
            margin: 0;
            padding: 0;
        }

        header {
            background: #162447;
            padding: 20px;
            text-align: center;
            font-size: 1.8em;
            font-weight: bold;
            color: #eaeaea;
            letter-spacing: 1px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.4);
        }

        .container {
            padding: 30px;
            max-width: 700px;
            margin: 40px auto;
            background: #1f1f3a;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.5);
        }

        h2 {
            margin-top: 0;
            border-bottom: 1px solid #444;
            padding-bottom: 8px;
            color: #f0f0f0;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
        }

        td {
            padding: 10px 5px;
            vertical-align: middle;
        }

        label {
            font-size: 1.05em;
            color: #cfcfcf;
        }

        input {
            width: 95%;
            padding: 8px;
            border-radius: 6px;
            border: 1px solid #444;
            background: #2d2d44;
            color: #eaeaea;
            font-size: 1em;
        }

        input:focus {
            outline: none;
            border-color: #5c7cfa;
            box-shadow: 0 0 5px rgba(92,124,250,0.5);
        }

        .form-section {
            margin-bottom: 25px;
        }
    </style>
</head>

<body>

<header>Finance Budgeting</header>

<div class="container">

    <h2>Account Information</h2>

    <form method="POST">
        <div class="form-section">
            <table>
                <tr>
                    <td width="150px"><label for="accountID">Account ID</label></td>
                    <td><input id="accountID" name="accountID"></td>
                </tr>
                <tr>
                    <td><label for="accountName">Name</label></td>
                    <td><input id="accountName" name="accountName"></td>
                </tr>
                <tr>
                    <td><label for="openingBalance">Opening Balance</label></td>
                    <td><input type="number" step="0.01" id="openingBalance" name="openingBalance"></td>
                </tr>
            </table>
        </div>
        <div>
            <table id="tranTable">
                <tr>
                    <th align="left">Transaction</th>
                    <th align="left">Date</th>
                </tr>
            </table>
        </div>
    </form>

</div>

</body>
</html>

)";


Finance ::Finance()
{
	m_cRef = 1; 
    m_iAccount = new Account(1000.0);
    g_memoryChecker.IncrementInstance(FinanceInf::ClassID);
}

Finance::~Finance()
{
    m_iAccount->Release();
    g_memoryChecker.DecrementInstance(FinanceInf::ClassID);
}

ULONG Finance::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Finance::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Finance::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == FinanceInf::m_iid)
    {
        *ppvObj = static_cast<FinanceInf::IFINANCE*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall Finance::UnitTest()
{
    StringInf ihtm;
    VariableInf iVar;
    MoeInf<IDATABASE, CLASSID::DATABASE> iDB;
    MoeInf<ITABLE, CLASSID::TABLE> iTbl;

    ihtm.Init();
    ihtm.BufferSize(4000);
    iDB.Init();

    if (SUCCEEDED(iDB->Load(L"finance.db")))
    {
        iDB->GetTable(L"modules", iTbl);
        iTbl->Get(L"accountID", ihtm);
    }

    Transaction* pNewTran = NULL;
    m_iAccount->GetTransaction(0, (IUnknown**)&pNewTran);
    pNewTran->Command(L"set amount -50.0");
    pNewTran->Print();

    m_iAccount->Update();

    iDB->GetTable(L"accounts", iTbl);
    m_iAccount->ID(iVar);

    iTbl->Set(L"accountID", iVar.GetString(), VLVariable::VAR_TYPE::TYPE_STRING);
    
    
    Command(L"account get name");
    GetReturnString(ihtm);
    iTbl->Set(L"accountName", (const wchar_t*)ihtm, VLVariable::VAR_TYPE::TYPE_STRING);    
    
    Command(L"account get balance");
    GetReturnString(ihtm);
    iTbl->Set(L"openingBalance", (const wchar_t*)ihtm, VLVariable::VAR_TYPE::TYPE_FLOAT);
    iDB->Save(L"new_finance.db");

    return S_OK;
}

HRESULT __stdcall Finance::HTMLPage(const wchar_t* szVarList, IUnknown* iunk)
{
    wstring htm;
    VLStringCollection varList;
    StringInf iHtml;

    iHtml.Attach(iunk);
    varList.Split(szVarList, L'&');
    htm = HTML_TEMPLATE;
    m_iAccount->HTMLPage(varList, htm);
    

    iHtml.Set(htm.c_str());
    return S_OK;
}

HRESULT __stdcall Finance::Command(const wchar_t* szCmd)
{
    HRESULT hr = E_FAIL;
    VLStringCollection cmds;
    wstring s;

    if (!szCmd || wcslen(szCmd) == 0)
        return E_INVALIDARG;

    cmds.Split(szCmd, L' ');
    cmds.ToLower(0);

     switch (szCmd[0])
    {
    case L'a':
    case L'A':
        if (cmds.Compare(0, L"account"))
        {
            hr = m_iAccount->Command(cmds.SubString(1, L' ').c_str());
            m_iAccount->GetReturnString(m_iRetStr);
        }
        break;
    }

    return hr;
}

HRESULT __stdcall Finance::GetReturnString(IUnknown** iStr)
{
    *iStr = (IUnknown*)m_iRetStr;
    return S_OK;
}

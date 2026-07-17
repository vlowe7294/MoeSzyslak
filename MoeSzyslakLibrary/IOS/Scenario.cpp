#include "pch.h"
#include "Scenario.h"

Scenario::Scenario()
{
    m_cRef = 1;

	m_pSelectedTrainer = NULL;
    m_pRecord = m_properties.NewVariable(L"record");
    m_pRecord->SetAsBool(FALSE);  

    m_pICFilePath = m_properties.NewVariable(L"IC File Path");


}

Scenario::~Scenario()
{
	if (m_pSelectedTrainer != NULL)
		m_pSelectedTrainer->Release();

}

ULONG Scenario::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Scenario::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Scenario::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

void Scenario::Command(wstring szCmd)
{
    
}

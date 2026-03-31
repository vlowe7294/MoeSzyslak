#include "pch.h"
#include "../Interfaces.h"

NeverwinterInf::NeverwinterInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

NeverwinterInf::~NeverwinterInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

bool NeverwinterInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;
	Attach();

	return m_iVar != NULL;
}

void NeverwinterInf::Attach()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(NEVERWINTER_IID, (void**)&m_iVar);
		m_iUnk->Release();
		return;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
	{
		m_iUnk->QueryInterface(NEVERWINTER_IID, (void**)&m_iVar);
	}
}

NeverwinterInf::INEVERWINTER* NeverwinterInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

bool NeverwinterInf::Dispose()
{
	if (m_iVar == NULL)
		return false;

	if (FAILED(m_iVar->Dispose()))
		return false;

	if (m_iVar->Release() > 0)
		return false;

	m_iVar = NULL;
	m_iUnk = NULL;
	return true;
}

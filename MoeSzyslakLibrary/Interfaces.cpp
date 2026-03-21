#include "pch.h"
#include "Interfaces.h"


const GUID TestingInf::m_iid =
{ 0x7c6da0f8, 0x84ae, 0x4e97, { 0x86, 0xf0, 0x13, 0xbb, 0x1e, 0x67, 0xc7, 0x13 } };

TestingInf::TestingInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

TestingInf::~TestingInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

bool TestingInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;
	Attach();

	if (m_iVar != NULL)
		m_iVar->AddRef();

	return m_iVar != NULL;
}

void TestingInf::Attach()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
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
		m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
		m_iUnk->Release();
	}
}

TestingInf::ITESTING* TestingInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}




InterfaceCollectionInf::InterfaceCollectionInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

InterfaceCollectionInf::~InterfaceCollectionInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

IINTERFACECOLLECTION* InterfaceCollectionInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

void InterfaceCollectionInf::Attach()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(CLASSID::INTERFACELIST, &m_iUnk);
		m_iUnk->QueryInterface(INTERFACECOLLECTION_IID, (void**)&m_iVar);
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
		m_iUnk->QueryInterface(INTERFACECOLLECTION_IID, (void**)&m_iVar);
	}
}

bool InterfaceCollectionInf::ForEach(IUnknown** iunk)
{
	if (m_iVar == NULL)
		return false;

	return m_iVar->ForEach(iunk) == S_OK;
}

bool InterfaceCollectionInf::Dispose()
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




CommsInf::CommsInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

CommsInf::~CommsInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

CommsInf::operator IUnknown* ()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(COMMGENERAL_IID, (void**)&m_iVar);
		m_iUnk->Release();
	}

	return m_iUnk;
}

ICOMMGENERALPAGE* CommsInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

void CommsInf::Attach()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(COMMGENERAL_IID, (void**)&m_iVar);
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
		m_iUnk->QueryInterface(COMMGENERAL_IID, (void**)&m_iVar);
		m_iUnk->Release();
	}
}
#include "../pch.h"
#include "../Interfaces.h"

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

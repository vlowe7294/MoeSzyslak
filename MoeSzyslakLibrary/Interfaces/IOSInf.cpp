#include "pch.h"
#include "../Interfaces.h"

IOSInf::IOSInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

IOSInf::~IOSInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

bool IOSInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;

	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(IOS_IID, (void**)&m_iVar);
		m_iUnk->Release();
		return true;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
	{
		m_iUnk->QueryInterface(IOS_IID, (void**)&m_iVar);
	}

	return m_iVar != NULL;
}

IOSInf::IIOS* IOSInf::operator->()
{
	if (m_iVar == NULL)
		Attach(NULL);

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

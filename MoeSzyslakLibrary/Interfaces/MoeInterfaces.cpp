#include "pch.h"
#include "..\Interfaces.h"

UserInf::UserInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

UserInf::~UserInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

void UserInf::Init()
{
	m_iUnk = NULL;
	CreateMoeSzyslakInterface(ClassID, &m_iUnk);
}

UserInf::operator IUnknown* ()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(USER_IID, (void**)&m_iVar);
		m_iUnk->Release();
	}

	return m_iUnk;
}

IUSER* UserInf::operator->()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(USER_IID, (void**)&m_iVar);
		m_iUnk->Release();
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
	{
		m_iUnk->QueryInterface(USER_IID, (void**)&m_iVar);
	}

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

#include "pch.h"
#include "../Interfaces.h"

GUID TripPlannerInf::m_iid =
{ 0x401e8b37, 0x4e1e, 0x4ac5, { 0xb4, 0x7c, 0x45, 0xfb, 0x45, 0x49, 0x4f, 0xf3 } };

TripPlannerInf::TripPlannerInf()
{
	m_iUnk = NULL;	
	m_iVar = NULL;
}

TripPlannerInf::~TripPlannerInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();	
}

bool TripPlannerInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;
	Attach();

	if (m_iVar != NULL)
		m_iVar->AddRef();

	return m_iVar != NULL;
}

void TripPlannerInf::Attach()
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

TripPlannerInf::ITRIPPLANNER* TripPlannerInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}



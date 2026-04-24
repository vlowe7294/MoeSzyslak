#include "../pch.h"
#include "../Interfaces.h"

GUID FinanceInf::m_iid = 
{ 0x3bad86fa, 0xdec6, 0x42c7, { 0x95, 0x5a, 0x20, 0x69, 0xb6, 0x37, 0xbf, 0xc9 } };

FinanceInf::FinanceInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

FinanceInf::~FinanceInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

void FinanceInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;


	if (m_iUnk == NULL)
	{
		Init();
		return;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();
}

void FinanceInf::Init()
{
	m_iUnk = NULL;

	if (m_iVar != NULL)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	CreateMoeSzyslakInterface(ClassID, &m_iUnk);

	if (m_iUnk == NULL)
		return;

	GetInterface();
	m_iUnk->Release();
}

void FinanceInf::GetInterface()
{
	m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
}

FinanceInf::IFINANCE* FinanceInf::operator->()
{
	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}
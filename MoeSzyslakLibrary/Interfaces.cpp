#include "pch.h"
#include "Interfaces.h"

const std::unordered_map<std::wstring, UINT> CLASS_NAMES =
{
	{ L"variable",			VariableInf::ClassID },
	{ L"interface list",	CLASSID::INTERFACELIST },
	{ L"area",				CLASSID::AREA },
	{ L"ios",				IOSInf::ClassID },
	{ L"finance",			FinanceInf::ClassID },
	{ L"testing",			TestingInf::ClassID },
	{ L"table",				CLASSID::TABLE },
	{ L"trip planner",		TripPlannerInf::ClassID },
	{ L"date time",			CLASSID::DATETIME },
	{ L"module",			CLASSID::MODULE },
	{ L"host",				CLASSID::HOST },
	{ L"database",			CLASSID::DATABASE },
	{ L"creature",			CLASSID::CREATURE },
	{ L"neverwinter",		NeverwinterInf::ClassID }
};

const GUID InterfaceCollectionInf::m_iid =
{ 0x19573a2d, 0x9df6, 0x4260, { 0xaa, 0x5b, 0xe5, 0xf5, 0xf1, 0xb8, 0x1d, 0x4c } };


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

InterfaceCollectionInf::IINTERFACECOLLECTION* InterfaceCollectionInf::operator->()
{
	Attach(m_iUnk);

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

void InterfaceCollectionInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;

	if (m_iUnk == NULL)
	{
		if (m_iVar != NULL)
		{
			m_iVar->Release();
			m_iVar = NULL;
		}

		CreateMoeSzyslakInterface(CLASSID::INTERFACELIST, &m_iUnk);
		m_iUnk->QueryInterface(InterfaceCollectionInf::m_iid, (void**)&m_iVar);
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
		m_iUnk->QueryInterface(InterfaceCollectionInf::m_iid, (void**)&m_iVar);
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




LogEntryInf::LogEntryInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
}

LogEntryInf::~LogEntryInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

void LogEntryInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;
	Attach();
}

ILOGENTRY* LogEntryInf::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

void LogEntryInf::Attach()
{
	if (m_iUnk == NULL)
	{
		CreateMoeSzyslakInterface(ClassID, &m_iUnk);
		m_iUnk->QueryInterface(LOGENTRY_IID, (void**)&m_iVar);
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
		m_iUnk->QueryInterface(LOGENTRY_IID, (void**)&m_iVar);
	}
}
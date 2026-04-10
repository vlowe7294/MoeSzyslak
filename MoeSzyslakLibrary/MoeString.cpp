#include "pch.h"
#include "Interfaces.h"

GUID JimboStringInf::m_iid =
{ 0x28b988b5, 0x3520, 0x4729, { 0xa6, 0x3b, 0x5, 0xeb, 0x57, 0xd0, 0xff, 0xb2 } };

JimboStringInf::JimboStringInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
	m_nBufSize = 255;
	m_pBuf = new wchar_t[255];
	memset(m_pBuf, 0, sizeof(wchar_t) * 255);

}

JimboStringInf::~JimboStringInf()
{
	delete m_pBuf;
	Dispose();
}

JimboStringInf::operator const wchar_t* ()
{
	int i = 0;

	m_pBuf[0] = L'\0';

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();

	if (m_iVar == NULL)
		return m_pBuf;

	m_iVar->Get(m_pBuf, 0, m_nBufSize - 5);
	return m_pBuf;
}

void JimboStringInf::GetInterface()
{
	if (m_iUnk != NULL)
		m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
}

void JimboStringInf::Dispose()
{
	if (m_iVar != NULL)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}
}






GUID StringInf::m_iid =
{ 0x76106157, 0x1218, 0x48bd, { 0xae, 0xa4, 0xcb, 0x53, 0xb9, 0x49, 0x97, 0x3e } };


StringInf::StringInf()
{
	m_iUnk = NULL;
	m_iVar = NULL;
	m_nBufSize = 255;
	m_pBuf = new wchar_t[255];
	memset(m_pBuf, 0, sizeof(wchar_t) * 255);
}

StringInf::~StringInf()
{
	Dispose();
	delete m_pBuf;
}

void StringInf::Dispose()
{
	if (m_iVar != NULL)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}
}

StringInf::ISTRING* StringInf::operator->()
{
	if (m_iUnk == NULL)
		Init();

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();

	return m_iVar;
}

void StringInf::Init()
{
	m_iUnk = NULL;
	Dispose();

	CreateMoeSzyslakInterface(ClassID, &m_iUnk);

	if (m_iUnk == NULL)
		return;

	GetInterface();
	m_iUnk->Release();
}

void StringInf::GetInterface()
{
	if (m_iUnk != NULL)
		m_iUnk->QueryInterface(m_iid, (void**)&m_iVar);
}

void StringInf::BufferSize(UINT nSize)
{
	wchar_t* pNew;

	if (nSize < m_nBufSize)
		return;

	pNew = new wchar_t[nSize];
	memcpy(pNew, m_pBuf, sizeof(wchar_t) * m_nBufSize);
	delete m_pBuf;
	m_pBuf = pNew;
	m_nBufSize = nSize;
}

void StringInf::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;
}

void StringInf::Set(LPCWSTR szFmtStr)
{
	if (m_iVar == NULL)
		GetInterface();

	if (m_iVar != NULL)
		m_iVar->Set(szFmtStr);

	BufferSize(lstrlenW(szFmtStr) + 5);
}

StringInf::operator const wchar_t* ()
{
	int i = 0;

	m_pBuf[0] = L'\0';

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();

	if (m_iVar == NULL)
		return m_pBuf;

	m_iVar->Get(m_pBuf, 0, m_nBufSize - 5);
	return m_pBuf;
}
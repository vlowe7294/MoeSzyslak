#include "pch.h"
#include "VLString.h"
#include <algorithm>
#include <format>

VLString::VLString()
{
	m_cRef = 1;
	m_buf = NULL;
	m_bufLen = 0;
}

VLString::~VLString()
{
	delete m_buf;
}

HRESULT __stdcall VLString::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	*ppvObj = NULL;

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == StringInf::m_iid)
	{
		*ppvObj = static_cast<StringInf::ISTRING*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall VLString::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall VLString::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall VLString::Set(LPCWSTR szFmtStr)
{
	m_str = szFmtStr;
	return S_OK;
}

HRESULT __stdcall VLString::Get(wchar_t* szStr, UINT ndx, UINT nMaxLen)
{
	szStr[0] = L'\0';

	if (m_buf != NULL)
	{
		m_str = m_buf;
		delete m_buf;
		m_buf = NULL;
	}

	if (ndx >= m_str.length())
		return S_OK;

	wcsncpy_s(szStr, nMaxLen, m_str.substr(ndx).c_str(), nMaxLen - 2);
	return S_OK;
}

HRESULT __stdcall VLString::Append(LPCWSTR szStr)
{
	m_str += szStr;
	return S_OK;
}

void VLString::SetBufferLen(UINT nLen)
{
	if (m_bufLen < nLen)
	{
		delete m_buf;
		m_bufLen = nLen;
		m_buf = new wchar_t[m_bufLen];
		m_buf[0] = L'\0';
	}
}

VLString::operator wchar_t* ()
{
	if (m_buf == NULL)
		SetBufferLen(255);

	return m_buf;
}

int VLString::Find(LPCWSTR szStr, int ndx)
{
	return m_str.find(szStr, ndx);
}

void VLString::MakeLowerCase()
{
	std::transform(m_str.begin(), m_str.end(), m_str.begin(), [](wchar_t c) { return std::tolower(c); });
}

void VLString::Replace(LPCWSTR szFrom, LPCWSTR szTo)
{
	size_t start_pos = 0;
	wstring from(szFrom), to(szTo);
	while ((start_pos = m_str.find(szFrom, start_pos)) != std::wstring::npos) 
	{
		m_str.replace(start_pos, from.length(), szTo);
		start_pos += to.length();
	}
}




VLStringCollection::VLStringCollection()
{
	
}

VLStringCollection::~VLStringCollection()
{

}

void VLStringCollection::Split(LPCWSTR szIn, wchar_t del)
{
	size_t len = wcslen(szIn);
	bool bQuoteFlag = false;
	wstring s;

	Clear();

	for (size_t i = 0; i < len; i++)
	{
		if (szIn[i] == del && !bQuoteFlag)
		{
			Add(s.c_str());
			s.clear();
		}
		else if (szIn[i] == L'\"')
		{
			bQuoteFlag = !bQuoteFlag;
		}
		else
			s += szIn[i];
	}

	Add(s.c_str());
}

void VLStringCollection::Clear()
{
	m_strings.Clear();
}

void VLStringCollection::Add(LPCWSTR szTag)
{
	VLString* pRet = new VLString();
	pRet->Set(szTag);

	m_strings.Add(pRet, szTag, StringInf::ClassID);
}

bool VLStringCollection::ForEach(wstring& str)
{
	StringInf iStr;
	bool bRet = (m_strings.ForEach((IUnknown**)&iStr) == S_OK);

	str = iStr;
	return bRet;
}

bool VLStringCollection::Compare(UINT ndx, LPCWSTR strToCmp)
{
	wstring s = Get(ndx);

	return s == strToCmp;
}

wstring VLStringCollection::Get(UINT ndx)
{
	VLString* pStr = NULL;
	m_strings.Get(ndx, (IUnknown**)&pStr);

	if (pStr != NULL)
	{
		return pStr->Get();
	}

	return L"";

}

void VLStringCollection::ToLower(int ndx)
{
	VLString* pStr = NULL;
	m_strings.Get(ndx, (IUnknown**)&pStr);

	if (pStr != NULL)
		pStr->MakeLowerCase();
}

bool VLStringCollection::Find(wstring strToFind)
{
	VLString* pStr = NULL;
	m_strings.GetByTag(strToFind.c_str(), (IUnknown**)&pStr);

	return (pStr != NULL);
}

wstring VLStringCollection::SubString(UINT ndx, wchar_t del)
{
	wstring ret;
	int i = 0;

	VLString* pStr = NULL;
	m_strings.Get(ndx, (IUnknown**)&pStr);

	while (pStr != NULL)
	{
		if (i > 0)
			ret += del;

		if (pStr->Find(L" ") >= 0)
			ret += L"\"";

		ret += pStr->Get();

		if (pStr->Find(L" ") >= 0)
			ret += L"\"";

		ndx++;
		i++;
		pStr = NULL;
		m_strings.Get(ndx, (IUnknown**)&pStr);
	}

	return ret;

}
#pragma once

#include "Interfaces.h"
#include <string>
#include "InterfaceCollection.h"

using namespace std;

class VLString : public StringInf::ISTRING
{
public:

	VLString();
	~VLString();

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Set(LPCWSTR szFmtStr);
	HRESULT __stdcall Get(wchar_t* szStr, UINT ndx, UINT nMaxLen);
	HRESULT __stdcall Append(LPCWSTR szStr);

	void SetBufferLen(UINT nLen);
	int Find(LPCWSTR szStr, int ndx = 0);
	void MakeLowerCase();
	void Replace(LPCWSTR szFrom, LPCWSTR szTo);

	operator wchar_t* ();
	
	inline wstring Get() { return m_str; };
	inline size_t GetLength() { return m_str.length(); };

private:
	int m_cRef;
	wstring m_str;
	wchar_t* m_buf;
	UINT m_bufLen;
};




class VLStringCollection
{
public:
	VLStringCollection();
	~VLStringCollection();
	void Split(LPCWSTR strIn, wchar_t del);
	bool ForEach(wstring& str);
	bool Compare(UINT ndx, LPCWSTR strToCmp);
	wstring Get(UINT ndx);
	bool Find(wstring strToFind);
	void Add(LPCWSTR szTag);
	wstring SubString(UINT ndx, wchar_t del);
	void Clear();
	void ToLower(int ndx);

	inline int GetCount() 
	{
		int nCnt = 0;
		m_strings.Count(&nCnt);
		return nCnt;
	};

private:
	InterfaceCollection m_strings;
};

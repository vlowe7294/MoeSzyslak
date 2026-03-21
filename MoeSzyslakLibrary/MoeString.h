#pragma once

#include <comdef.h>

class JimboStringInf
{
public:
	static const UINT ClassID = 345553;

	struct IJIMBOSTRING : public IUnknown
	{
		virtual HRESULT __stdcall Invoke(IUnknown* iParam) = 0;
		virtual HRESULT __stdcall Get(wchar_t* szStr, UINT ndx, UINT nMaxLen) = 0;
		virtual HRESULT __stdcall Set(LPCWSTR szStr) = 0;
		virtual HRESULT __stdcall Replace(LPCWSTR oldStr, LPCWSTR newStr) = 0;
		virtual HRESULT __stdcall Append(LPCWSTR szStr) = 0;
	};

	// {28B988B5-3520-4729-A63B-05EB57D0FFB2}
	static GUID m_iid;

	JimboStringInf();
	~JimboStringInf();

	inline operator IUnknown** () { return &m_iUnk; };
	operator const wchar_t* ();

private:
	IUnknown* m_iUnk;
	IJIMBOSTRING* m_iVar;
	UINT m_nBufSize;
	wchar_t* m_pBuf;

	void Dispose();
	void GetInterface();

};

class StringInf
{
public:
	static const UINT ClassID = 345553;

	struct ISTRING : public IUnknown
	{
		virtual HRESULT __stdcall Set(LPCWSTR szFmtStr) = 0;	
		virtual HRESULT __stdcall Get(wchar_t* szStr, UINT ndx, UINT nMaxLen) = 0;
		virtual HRESULT __stdcall Append(LPCWSTR szStr) = 0;
	};	

	// {76106157-1218-48BD-AEA4-CB53B949973E}
	static GUID m_iid;

	StringInf();
	~StringInf();
	void Dispose();
	void Init();
	void BufferSize(UINT nSize);
	void Attach(IUnknown* iunk);
	
	ISTRING* operator->();
	operator const wchar_t* ();
	inline operator IUnknown* () { return m_iUnk; };
	inline operator IUnknown** () { return &m_iUnk; };
private:
	IUnknown* m_iUnk;
	ISTRING* m_iVar;
	UINT m_nBufSize;
	wchar_t* m_pBuf;

	void GetInterface();
	
};

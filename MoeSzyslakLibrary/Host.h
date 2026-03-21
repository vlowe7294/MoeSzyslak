#pragma once

#include "Interfaces.h"

class Session : public IUnknown
{
public:
	Session();
	~Session();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void GetLastAccess(IUnknown** iunk);
	void ParseCommand(LPCWSTR cmd);


	inline UINT GetID() { return m_nID; };

private:
	int m_cRef;
	UINT m_nID;
	static UINT m_nNextID;
	MoeInf<IDATETIME, CLASSID::DATETIME> m_lastAccess;
	StringInf m_strInf;
};



class Host : public IHOST
{
public:
	static const UINT m_nClassID = CLASSID::HOST;
	
	Host();
	~Host();

	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall ParseCommand(UINT* nSession, LPCWSTR cmd);

private:
	int m_cRef;
	MoeInf<IINTERFACECOLLECTION, CLASSID::INTERFACELIST> m_sessions;

};

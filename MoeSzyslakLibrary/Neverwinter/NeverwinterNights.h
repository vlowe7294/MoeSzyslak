#pragma once

#include "..\VLString.h"
#include "..\VLVariable.h"
#include "Module.h"

class NeverwinterNights : public NeverwinterInf::INEVERWINTER
{
public:
	NeverwinterNights();
	~NeverwinterNights();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall UnitTest();
	HRESULT __stdcall NWNExit();
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall Properties(IUnknown** iProp);
	HRESULT __stdcall Dispose();

private:
	int m_cRef;
	Module* m_iModule;	
	typedef HRESULT(__cdecl* DO_FUN)(const wchar_t* szParam);
	std::map<std::wstring, DO_FUN> m_funcList;
	StringInf m_iReturnStr;
	IUnknown* m_iActive;
	VariableCollection m_properties;
	UserInf m_iUser;

	void Start();
	void Load();
	void Save();
};

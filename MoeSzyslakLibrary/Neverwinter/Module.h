#pragma once
#include "..\Interfaces.h"
#include "..\VLVariable.h"
#include "Area.h"
#include "..\Utilities\Database.h"

class Module : public IMODULE
{
public:

	Module();
	~Module();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall Command(const wchar_t* szCmd, IUnknown* iRetStr);
	HRESULT __stdcall Save(IUnknown* iDB);

	void Load(Database& db);

private:
	int m_cRef;
	VariableCollection* m_pProperties;
	VLVariable* m_pName;
	InterfaceCollection* m_pAreas;
	VLVariable* m_pAreasVar;
};

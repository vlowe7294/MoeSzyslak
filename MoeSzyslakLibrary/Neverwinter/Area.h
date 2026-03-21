#pragma once

#include "../VLVariable.h"
#include "Placeable.h"
#include "Creature.h"
#include "..\Utilities\Database.h"

class Area : public IAREA
{
public:
	Area();
	~Area();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall Command(const wchar_t* szCmd, IUnknown* iRetStr);
	HRESULT __stdcall Tick(int nSec);

	Placeable& GetPlaceable(int ndx);
	void Save(Table& tbl);
	void Load(Table& tbl);
	

private:
	int m_cRef;
	VLVariable* m_pName;
	InterfaceCollectionInf m_contents;
	VariableCollection* m_pProperties;

};

#pragma once

#include "..\VLString.h"
#include "..\VLVariable.h"
#include "Module.h"

class Room : public IUnknown
{
public:
	enum DIRECTION
	{
		NORTH,
		SOUTH,
		EAST,
		WEST,
		UP,
		DOWN,
		MAX_DIRECTIONS
	};

	static DIRECTION OPPOSITE_DIRECTION[MAX_DIRECTIONS];
		
	Room();
	~Room();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void AddExit(DIRECTION dir);
	inline void Description(wstring desc) { m_description = desc; };

private:
	static UINT m_lastID;
	int m_cRef;
	UINT m_id;
	Room* m_exits[MAX_DIRECTIONS];
	wstring m_description;
};

class NeverwinterNights : public NeverwinterInf::INEVERWINTER
{
public:
	NeverwinterNights();
	~NeverwinterNights();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall UnitTest();
	
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall Properties(IUnknown** iProp);
	HRESULT __stdcall Dispose();
	HRESULT __stdcall Save(const wchar_t* szFilePath);

	void ShowIntro();

private:
	int m_cRef;
	Module* m_iModule;	
	typedef HRESULT(__cdecl* DO_FUN)(const wchar_t* szParam);
	std::map<std::wstring, DO_FUN> m_funcList;
	StringInf m_iReturnStr;
	IUnknown* m_iActive;
	VariableCollection m_properties;
	UserInf m_iUser;
	wstring m_intro;
	Room* m_pStartingRoom;

	void Start();
	void Load();
	
};

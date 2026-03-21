#pragma once
#include "Utilities\Database.h"

class User : public IUSER
{
public:
	User();
	~User();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall UnitTest();
	
	void Login();
	void SaveUserList(Database& db);

private:
	int m_cRef;
	VariableCollection* m_pProperties;
	VLVariable* m_pName;
	VLVariable* m_pPassword;
	VLVariable* m_pIsLoggedIn;
	VLVariable* m_pIsAdmin;
	MoeInf<IDATETIME, CLASSID::DATETIME> m_iLastLogin;
	StringInf m_iReturnStr;

	static InterfaceCollection m_masterUserList;

	bool Compare(User& usr);
};

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
	HRESULT __stdcall Login();
	HRESULT __stdcall SaveUserList(IUnknown* iDB);
	HRESULT __stdcall UnitTest();	
	HRESULT __stdcall AddToMasterList(const wchar_t* szTag, const wchar_t* szPassword);

	void LoadUserList(Database& db);
	void Copy(User& usr);
	

private:
	int m_cRef;
	VariableCollection* m_pProperties;
	VLVariable* m_pName;
	VLVariable* m_pPassword;
	VLVariable* m_pIsLoggedIn;
	VLVariable* m_pIsAdmin;
	VLVariable* m_pEmail;
	MoeInf<IDATETIME, CLASSID::DATETIME> m_iLastActive;
	StringInf m_iReturnStr;

	static InterfaceCollection m_masterUserList;

	bool Compare(User& usr);
};

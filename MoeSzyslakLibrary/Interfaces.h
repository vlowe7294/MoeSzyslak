#pragma once

#include <comdef.h>
#include <stdexcept>
#include "MoeString.h"
#include "Interfaces\TripPlannerInf.h"
#include "Interfaces\IOSInf.h"
#include "Interfaces\FinanceInf.h"
#include "Interfaces\NeverwinterInf.h"

struct IINTERFACECOLLECTION : public IUnknown
{
	virtual HRESULT __stdcall Add(IUnknown* iObj, const wchar_t* szTag, UINT nClassID) = 0;
	virtual HRESULT __stdcall Get(int ndx, IUnknown** iunk) = 0;
	virtual HRESULT __stdcall Count(int* nCnt) = 0;
	virtual HRESULT __stdcall Remove(int ndx) = 0;
	virtual HRESULT __stdcall GetByTag(const wchar_t* szTag, IUnknown** iunk) = 0;
	virtual HRESULT __stdcall ForEach(IUnknown** pObj) = 0;
	virtual HRESULT __stdcall Clear() = 0;
	virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
	virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
	virtual HRESULT __stdcall Dispose() = 0;
	virtual HRESULT __stdcall UnitTest(IUnknown* iTst) = 0;
};

#include "Interfaces/VariableInfCollection.h"


enum CLASSID
{
	INVALIDCLASS	= 0,
	LOGENTRY		= 134569,
	INTERFACELIST	= 184004,
	AREA			= 262241,
	TABLE			= 416737,
	TRIPPLANNER		= 507734,
	DATETIME		= 568996,
	MODULE			= 628950,
	HOST			= 647491,
	DATABASE		= 781903,	
	CREATURE		= 924610
};

// {57779E06-1468-47C9-859A-49E07CEF5F8C}
static const GUID HOST_IID =
{ 0x57779e06, 0x1468, 0x47c9, { 0x85, 0x9a, 0x49, 0xe0, 0x7c, 0xef, 0x5f, 0x8c } };

// {19573A2D-9DF6-4260-AA5B-E5F5F1B81D4C}
static const GUID INTERFACECOLLECTION_IID =
{ 0x19573a2d, 0x9df6, 0x4260, { 0xaa, 0x5b, 0xe5, 0xf5, 0xf1, 0xb8, 0x1d, 0x4c } };

// {3D2AD9D0-D5E1-4916-AAA5-8BD482662919}
static const GUID DATETIME_IID =
{ 0x3d2ad9d0, 0xd5e1, 0x4916, { 0xaa, 0xa5, 0x8b, 0xd4, 0x82, 0x66, 0x29, 0x19 } };

// {8346DA8D-9EFD-436A-9545-8D324DE0E43E}
static const GUID CREATURE_IID =
{ 0x8346da8d, 0x9efd, 0x436a, { 0x95, 0x45, 0x8d, 0x32, 0x4d, 0xe0, 0xe4, 0x3e } };

// {9E39F153-0811-498B-B72A-5C810D0141A0}
static const GUID AREA_IID =
{ 0x9e39f153, 0x811, 0x498b, { 0xb7, 0x2a, 0x5c, 0x81, 0xd, 0x1, 0x41, 0xa0 } };

// {88C90DC5-A2A6-44B9-A146-CCB1DB6B651B}
static const GUID MODULE_IID =
{ 0x88c90dc5, 0xa2a6, 0x44b9, { 0xa1, 0x46, 0xcc, 0xb1, 0xdb, 0x6b, 0x65, 0x1b } };

// {AEC035FA-9488-499C-9966-92CCEE7464AC}
static const GUID NEVERWINTER_IID =
{ 0xaec035fa, 0x9488, 0x499c, { 0x99, 0x66, 0x92, 0xcc, 0xee, 0x74, 0x64, 0xac } };

// {AC597284-A906-482F-9BE9-C4E13AB43E50}
static const GUID LOGENTRY_IID =
{ 0xac597284, 0xa906, 0x482f, { 0x9b, 0xe9, 0xc4, 0xe1, 0x3a, 0xb4, 0x3e, 0x50 } };

// {5EB13971-FBC0-44EC-A70B-1D07EC498168}
static const GUID TABLE_IID =
{ 0x5eb13971, 0xfbc0, 0x44ec, { 0xa7, 0xb, 0x1d, 0x7, 0xec, 0x49, 0x81, 0x68 } };

// {905AF8A2-8742-45E7-86EA-240B54E8BA4C}
static const GUID DATABASE_IID =
{ 0x905af8a2, 0x8742, 0x45e7, { 0x86, 0xea, 0x24, 0xb, 0x54, 0xe8, 0xba, 0x4c } };

// {307A069A-5468-4749-80FD-B86CD4EE3990}
static const GUID IOS_IID =
{ 0x307a069a, 0x5468, 0x4749, { 0x80, 0xfd, 0xb8, 0x6c, 0xd4, 0xee, 0x39, 0x90 } };

// {7CDABB76-B191-42BB-829A-C97EE317EC37}
static const GUID COMMGENERAL_IID =
{ 0x7cdabb76, 0xb191, 0x42bb, { 0x82, 0x9a, 0xc9, 0x7e, 0xe3, 0x17, 0xec, 0x37 } };

// {3B1537E5-9E5D-4053-8510-1CEB51543F32}
static const GUID USER_IID =
{ 0x3b1537e5, 0x9e5d, 0x4053, { 0x85, 0x10, 0x1c, 0xeb, 0x51, 0x54, 0x3f, 0x32 } };



struct IHOST : public IUnknown
{
	virtual HRESULT __stdcall ParseCommand(UINT* nSession, LPCWSTR cmd) = 0;
};

struct IDATETIME : public IUnknown
{
	virtual HRESULT __stdcall GetDifference(IUnknown* iSubAmt, int* nSec) = 0;
};

struct ICREATURE : public IUnknown
{
	virtual HRESULT __stdcall Tick(int nSec) = 0;
};

struct IAREA : public IUnknown
{
	virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
	virtual HRESULT __stdcall Command(const wchar_t* szCmd, IUnknown* iRetStr) = 0;
	virtual HRESULT __stdcall Tick(int nSec) = 0;
};

struct IMODULE : public IUnknown
{
	virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
	virtual HRESULT __stdcall Command(const wchar_t* szCmd, IUnknown* iRetStr) = 0;
	virtual HRESULT __stdcall Save(IUnknown* iDB) = 0;
};

struct ILOGENTRY : public IUnknown
{
	virtual HRESULT __stdcall Properties(IUnknown** iProp) = 0;
	virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
	virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
};

struct ITABLE : public IUnknown
{
	virtual HRESULT __stdcall NewRow() = 0;
	virtual HRESULT __stdcall Set(const wchar_t* colName, const wchar_t* strVal, UINT nType) = 0;
};

struct IDATABASE : public IUnknown
{
	virtual HRESULT __stdcall GetTable(const wchar_t* szNme, IUnknown** iTbl) = 0;
	virtual HRESULT __stdcall Load(const wchar_t* szFileNme) = 0;
	virtual HRESULT __stdcall Save(const wchar_t* szFileNme) = 0;
};

struct IUSER : public IUnknown
{
	virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
	virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
	virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
	virtual HRESULT __stdcall UnitTest() = 0;
};

struct ICOMMGENERALPAGE : public IUnknown
{
	virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
	virtual HRESULT __stdcall UnitTest() = 0;
	virtual HRESULT __stdcall Commit() = 0;
};

void CreateMoeSzyslakInterface(UINT id, IUnknown** iunk);

template<class T, CLASSID id> class MoeInf
{
public:
	MoeInf();
	~MoeInf();
	void Init();
	bool Verify();
	void Attach(IUnknown* iunk);
	void Dispose();
	void Attach();
	void GetInterface();

	T* operator->();
	inline operator IUnknown** () { return &m_iUnk; };
	inline operator IUnknown* () { return m_iUnk; };
	inline operator T* () { return m_iVar; };

private:
	T* m_iVar;
	IUnknown* m_iUnk;
};


template<class T, CLASSID id>MoeInf<T, id>::MoeInf()
{
	m_iVar = NULL;
	m_iUnk = NULL;
}

template<class T, CLASSID id> MoeInf<T, id>::~MoeInf()
{
	if (m_iVar != NULL)
		m_iVar->Release();
}

template<class T, CLASSID id> void MoeInf<T, id>::Attach()
{
	if (m_iUnk == NULL)
	{
		Init();
		return;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();
}

template<class T, CLASSID id> void MoeInf<T, id>::Attach(IUnknown* iunk)
{
	m_iUnk = iunk;

	if (m_iUnk == NULL)
	{
		Init();
		return;
	}

	if (m_iVar != NULL && m_iVar != m_iUnk)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

	if (m_iVar == NULL)
		GetInterface();

}

template<class T, CLASSID id> void MoeInf<T, id>::Init()
{
	m_iUnk = NULL;
	Dispose();

	CreateMoeSzyslakInterface(id, &m_iUnk);

	if (m_iUnk == NULL)
		return;

	GetInterface();
	m_iUnk->Release();
}

template<class T, CLASSID id> T* MoeInf<T, id>::operator->()
{
	Attach();

	if (m_iVar == NULL)
		throw std::runtime_error("Invalid Interface");

	return m_iVar;
}

template<class T, CLASSID id> void MoeInf<T, id>::GetInterface()
{
	switch (id)
	{

	case CLASSID::LOGENTRY:
		m_iUnk->QueryInterface(LOGENTRY_IID, (void**)&m_iVar);
		break;

	case CLASSID::INTERFACELIST :
		m_iUnk->QueryInterface(INTERFACECOLLECTION_IID, (void**)&m_iVar);
		break;

	case CLASSID::AREA:
		m_iUnk->QueryInterface(AREA_IID, (void**)&m_iVar);
		break;

	case CLASSID::TABLE:
		m_iUnk->QueryInterface(TABLE_IID, (void**)&m_iVar);
		break;

	case CLASSID::DATETIME:
		m_iUnk->QueryInterface(DATETIME_IID, (void**)&m_iVar);
		break;

	case CLASSID::HOST:
		m_iUnk->QueryInterface(HOST_IID, (void**)&m_iVar);
		break;

	case CLASSID::MODULE:
		m_iUnk->QueryInterface(MODULE_IID, (void**)&m_iVar);
		break;

	case CLASSID::DATABASE:
		m_iUnk->QueryInterface(DATABASE_IID, (void**)&m_iVar);
		break;

	case CLASSID::CREATURE:
		m_iUnk->QueryInterface(CREATURE_IID, (void**)&m_iVar);
		break;
	}
}

template<class T, CLASSID id> bool MoeInf<T, id>::Verify()
{
	return m_iVar != NULL;
}

template<class T, CLASSID id> void MoeInf<T, id>::Dispose()
{
	if (m_iVar != NULL)
	{
		m_iVar->Release();
		m_iVar = NULL;
	}

}



class TestingInf
{
public:
	static const UINT ClassID = 398981;

	// {7C6DA0F8-84AE-4E97-86F0-13BB1E67C713}
	static const GUID m_iid;

	struct ITESTING : public IUnknown
	{
		virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall RunTest(UINT nClassID) = 0;
		virtual HRESULT __stdcall VerifyVariable(LPCWSTR varName, LPCWSTR val) = 0;
		virtual HRESULT __stdcall VerifyHResult(HRESULT hr, LPCWSTR szMsg) = 0;
		virtual HRESULT __stdcall Message(LPCWSTR szMsg) = 0;
		virtual HRESULT __stdcall GetTestData(LPCWSTR szName, IUnknown* iStr) = 0;
		virtual HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	TestingInf();
	~TestingInf();
	bool Attach(IUnknown* iunk);
	ITESTING* operator->();	

private:
	IUnknown* m_iUnk;
	ITESTING* m_iVar;

	void Attach();
};







class InterfaceCollectionInf
{
public:
	InterfaceCollectionInf();
	~InterfaceCollectionInf();
	void Attach();
	bool ForEach(IUnknown** iunk);
	bool Dispose();

	IINTERFACECOLLECTION* operator->();
	inline operator IUnknown* () { return m_iUnk; };
	inline operator IUnknown** () { return &m_iUnk; };

private:
	IUnknown* m_iUnk;
	IINTERFACECOLLECTION* m_iVar;
};




class UserInf
{
public:
	static const UINT ClassID = 325850;

	UserInf();
	~UserInf();
	void Init();

	IUSER* operator->();
	operator IUnknown* ();

	inline void Attach(IUnknown* iunk) { m_iUnk = iunk; };
	inline operator IUnknown** () { return &m_iUnk; };

private:
	IUnknown* m_iUnk;
	IUSER* m_iVar;
};



class CommsInf
{
public:
	static const UINT ClassID = 197619;

	CommsInf();
	~CommsInf();	
	void Attach();

	operator IUnknown* ();
	ICOMMGENERALPAGE* operator->();
	inline operator IUnknown** () { return &m_iUnk; };

private:
	IUnknown* m_iUnk;
	ICOMMGENERALPAGE* m_iVar;
};







struct LOCATION
{
	double x;
	double y;
	double z;
};

void VersionAsString(IUnknown* iunk);



class JimboJonesLibrary
{
public:
	static UINT GetJimboLibraryVersion();
	static const UINT MinVersion = 1465;
	static IUnknown* CreateInterface(UINT nClassID);

private:
	typedef UINT(__cdecl* PROC_GETLIBRARYVERSION)();
	typedef HRESULT(__cdecl* PROC_CREATEINTERFACE)(UINT nClassID, IUnknown** iunk);

	static HMODULE m_hModule;
	static void Init();

	static PROC_GETLIBRARYVERSION m_pGetLibraryVersion;
	static PROC_CREATEINTERFACE m_pCreateInterface;
};
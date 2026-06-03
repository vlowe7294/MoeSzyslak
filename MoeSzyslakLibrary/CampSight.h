#pragma once
#include "VLString.h"
#include <atomic>
#include "Utilities/Database.h"

class Site : public IUnknown
{
public:
	Site(wstring nme, double lat, double lon);
	~Site();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	void Print();

private:
	LONG m_cRef;
	uint32_t m_id;
	wstring m_name;
	double m_latitude;
	double m_longitude;
};




class CampArea : public IUnknown
{
public:
	CampArea(wstring nme);
	~CampArea();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	void Print();
	void Save(Database& db);
	void Load(Database& db);

	inline InterfaceCollection& GetSites() { return m_sites; }

private:
	LONG m_cRef;
	wstring m_name;
	InterfaceCollection m_sites;
};



class CampSight : public ICAMPSIGHT
{
public:
	CampSight();
	~CampSight();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();	
	HRESULT __stdcall GetTester(IUnknown** iTester);
	HRESULT __stdcall AddSite(const wchar_t* szArea, const wchar_t* szSite, double lat, double lon);
	HRESULT __stdcall UnitTest();

private:
	LONG m_cRef;
	
	static TestingInf m_test;
	InterfaceCollection m_areas;
};

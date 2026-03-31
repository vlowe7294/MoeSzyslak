#pragma once

#include "Interfaces.h"
#include "VLString.h"


class Stop : public IUnknown
{
public:
	Stop();
	~Stop();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	inline void Start(const wchar_t* szStart) { m_start = szStart; }

private:
	int m_cRef;
	wstring m_start;
};



class TripPlanner : public TripPlannerInf::ITRIPPLANNER
{
public:

	TripPlanner();
	~TripPlanner();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall UnitTest();

	Stop* AddStop();

	inline void Name(const wchar_t* name) { m_name = name; }

private:
	int m_cRef;
	wstring m_name;
	Stop* m_pStop;
};

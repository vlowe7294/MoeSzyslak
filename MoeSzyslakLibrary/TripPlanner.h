#pragma once

#include "Interfaces.h"

class TripPlanner : public TripPlannerInf::ITRIPPLANNER
{
public:

	TripPlanner();
	~TripPlanner();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

private:
	int m_cRef;
};

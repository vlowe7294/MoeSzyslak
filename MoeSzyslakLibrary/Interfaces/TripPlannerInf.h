#pragma once

#include <comdef.h>

class TripPlannerInf
{
public:
	static const UINT ClassID = 507734;

	struct ITRIPPLANNER : public IUnknown
	{
		virtual HRESULT __stdcall Properties(IUnknown** iProp) = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall AddStop(IUnknown** iStop) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	// {401E8B37-4E1E-4AC5-B47C-45FB45494FF3}
	static GUID m_iid;
		
	TripPlannerInf();
	~TripPlannerInf();
	bool Attach(IUnknown* iunk);
	ITRIPPLANNER* operator->();

private:
	IUnknown* m_iUnk;
	ITRIPPLANNER* m_iVar;

	void Attach();
};

void CreateMoeSzyslakInterface(UINT id, IUnknown** iunk);

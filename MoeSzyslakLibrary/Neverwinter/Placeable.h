#pragma once

#include "../VLString.h"

class Placeable : public IUnknown
{
public:
	Placeable();
	~Placeable();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	void Tick(int nSec);

	inline wstring GetName() { return m_name; };
	inline void SetName(LPCWSTR szName) { m_name = szName; }

private:
	int m_cRef;
	wstring m_name;
	int m_nWater;
	int m_nWaterCapacity;
	int m_regenRate;
	int m_nSecCount;
};

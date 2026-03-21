#pragma once

#include "../VLString.h"

class Creature : public ICREATURE
{
public:
	enum FACTION
	{
		FACTION_NEUTRAL,
		FACTION_EXPLORER,
		FACTION_GUARD
	};
	Creature();
	~Creature();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Tick(int nSec);

	void AddConnectedObject(void* pObject, int nTravelSec);

	inline wstring GetName() { return m_name; };
	inline void SetName(LPCWSTR szName) { m_name = szName; }
	inline void GetLocation(LOCATION& loc)
	{
		memcpy(&loc, &m_location, sizeof(LOCATION));
	}

	inline void SetLocation(LOCATION& loc)
	{
		memcpy(&m_location, &loc, sizeof(LOCATION));
	}

private:
	int m_cRef;
	wstring m_name;
	int m_thirst;
	int m_thirstRate;
	int m_nCount;
	LOCATION m_location;
	void* m_pConnectedObjects[10];
	int m_nTravelTimes[10];
	FACTION m_faction;
	void* m_pMoveTowards;
	int m_nMoveTime;
};

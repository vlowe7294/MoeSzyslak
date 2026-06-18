#pragma once

#include "../VLString.h"
#include "..\Interfaces.h"

struct NEEDS
{
	int amt;
	int rate;
	int count;
};

class Creature : public ICREATURE
{
public:
	enum FACTION
	{
		FACTION_NEUTRAL,
		FACTION_EXPLORER,
		FACTION_GUARD
	};

	enum CLASS
	{
		CLASS_NONE,
		CLASS_WARRIOR,
		CLASS_MAGE,
		CLASS_ROGUE,
		CLASS_MAX
	};

	
	

	Creature(const wchar_t* szName);
	~Creature();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	virtual HRESULT __stdcall Tick(int nSec);

	void AddConnectedObject(void* pObject, int nTravelSec);
	void SetArea(IUnknown* iArea);
	
	inline wstring GetName() { return m_name; };
	inline void SetName(LPCWSTR szName) { m_name = szName; }
	

protected:
	ULONG m_cRef;
	wstring m_name;
	CLASS m_class;

	NEEDS m_thirst;
	NEEDS m_hunger;
	int m_courage;

	IUnknown* m_pArea;

	void* m_pConnectedObjects[10];
	int m_nTravelTimes[10];
	FACTION m_faction;
	void* m_pMoveTowards;
	int m_nMoveTime;	
};

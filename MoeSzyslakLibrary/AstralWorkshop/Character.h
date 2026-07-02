#pragma once

#include "../Neverwinter/Creature.h"

class Character : public Creature
{
public:
	enum BACKGROUND
	{
		BACKGROUND_NONE,
		BACKGROUND_NOBLE,
		BACKGROUND_PEASANT,
		BACKGROUND_OUTLAW,
		BACKGROUND_MAX
	};

	enum ACTIVITY
	{
		ACTIVITY_IDLE,
		ACTIVITY_ON_QUEST,
		ACTIVITY_RETURNING,
		ACTIVITY_RESTING
	};

	Character(const wchar_t* szName, CLASS nClass, BACKGROUND nBkgrnd);
	~Character();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT CreateCharacter(wstring strCmd, wstring& strOut);
	HRESULT Tick(int nSec);
	void Save(ITABLE* iTbl);
	void Load(ITABLE* iTbl);
	HRESULT UnitTest();

	inline bool IsComplete() { return m_bComplete; }

private:
	bool m_bComplete;
	BACKGROUND m_background;
	ACTIVITY m_activity;
	int m_nEditStep;
	int m_fatigue; // 0 = fully rested, 100 = exhausted
	int m_restTimeRemaining;
	int m_heartbeat; 

};

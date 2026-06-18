#include "..\pch.h"
#include "Character.h"
#include "..\VLString.h"
#include "../Neverwinter/Area.h"

Character::Character(const wchar_t* szName, CLASS nClass, BACKGROUND nBkgrnd) : Creature(szName)
{
	m_bComplete = false;
	m_activity = ACTIVITY_IDLE;
	m_background = nBkgrnd;
	m_class = nClass;
	m_nEditStep = 0;
	m_fatigue = 0;
	m_restTimeRemaining = 0;
	m_heartbeat = 0;
}

Character::~Character()
{

}

HRESULT Character::CreateCharacter(wstring strCmd, wstring& strOut)
{
	int n = 0;

	switch (m_nEditStep)
	{
	case 0:
		strOut = L"Welcome to character creation.\nWhat is your character's name?";
		m_nEditStep = 1;
		m_name.clear();
		break;

	case 1:
		if (strCmd.empty())
		{
			strOut = L"What is your character's name?";
			return S_OK;
		}

		m_nEditStep = 2;
		m_name = strCmd;
		strOut = L"1) Warrior\n2) Mage\n3) Rogue\nWhat class do you want " + m_name + L" to be?";
		break;

	case 2:
		n = _wtoi(strCmd.c_str());

		if (n <= CLASS_NONE || n >= CLASS_MAX)
		{
			strOut = L"1) Warrior\n2) Mage\n3) Rogue\nWhat class do you want " + m_name + L" to be?";
			m_class = CLASS_NONE;
			return S_OK;
		}

		m_class = (CLASS)n;
		m_nEditStep = 3;
		strOut = L"1) Noble\n2) Peasant\n3) Outlaw\nWhat do you want " + m_name + L"'s background to be?";
		break;

	case 3:
		n = _wtoi(strCmd.c_str());

		if (n <= BACKGROUND_NONE || n >= BACKGROUND_MAX)
		{
			strOut = L"1) Noble\n2) Peasant\n3) Outlaw\nWhat do you want " + m_name + L"'s background to be?";
			m_background = BACKGROUND_NONE;
			return S_OK;
		}

		m_background = (BACKGROUND)n;
		m_nEditStep = 4;
		strOut = m_name + L" character created.  Enter command:";

		m_bComplete = true;
		break;
	}

	return S_OK;
}

HRESULT Character::Tick(int nSec)
{
	Dice d(100, 202606081256);
	m_heartbeat += nSec;

	while (m_heartbeat >= 60)
	{
		if (m_activity == ACTIVITY_RESTING)
		{
			m_restTimeRemaining -= 60;

			// Recover fatigue
			m_fatigue--; // 1 fatigue per minute

			if (m_fatigue < 0)
				m_fatigue = 0;

			if (m_restTimeRemaining <= 0)
			{
				m_activity = ACTIVITY_IDLE;
				m_restTimeRemaining = 0;
				wprintf(L"%s feels rested\n", m_name.c_str());
			}

		}
		else 
		{
			m_fatigue++; // +1 fatigue per minute
			m_heartbeat -= 60;

			// If extremely tired, rest immediately
			if (m_fatigue > 80)
				m_activity = ACTIVITY_RESTING;
			// If moderately tired, chance to rest
			else if (m_fatigue > 50 && d.Roll() < 10)
				m_activity = ACTIVITY_RESTING;

			if (m_activity == ACTIVITY_RESTING)
			{
				wprintf(L"%s begins resting\n", m_name.c_str());
				int base = 3600; // 1 hour
				int extra = (m_fatigue / 50) * 3600; // +1 hour if fatigue > 50
				m_restTimeRemaining = base + extra;
			}
		}
	}

	return Creature::Tick(nSec);
	
}

void Character::Save(ITABLE* iTbl)
{
	iTbl->Set(L"name", m_name.c_str(), VLVariable::TYPE_STRING);
	iTbl->Set(L"class", std::to_wstring(m_class).c_str(), VLVariable::TYPE_INT);
	iTbl->Set(L"background", std::to_wstring(m_background).c_str(), VLVariable::TYPE_INT);
}

void Character::Load(ITABLE* iTbl)
{
	StringInf strVal;

	strVal.Init();

	int nVal;
	iTbl->Get(L"name", strVal);
	m_name = strVal;
	
	iTbl->Get(L"class", strVal);
	nVal = _wtoi(strVal);
	m_class = (CLASS)nVal;

	iTbl->Get(L"background", strVal);
	nVal = _wtoi(strVal);
	m_background = (BACKGROUND)nVal;
	m_bComplete = true;
}

HRESULT Character::UnitTest()
{
	if (m_pArea != NULL)
	{
		Area* pArea = NULL;
		m_pArea->QueryInterface(AREA_IID, (void**)&pArea);
		Quest* pQst = new Quest(L"", 0, 0);

		pQst->Copy(pArea->GetQuest());
		pQst->Randomize();
		wprintf(L"%s has departed on a quest: \"%s\"\n", m_name.c_str(), pQst->Name().c_str());

		pArea->Release();
		delete pQst;
	}


	for (int i = 0; i < 100; i++)
	{
		Tick(100);
	}

	return S_OK;
}

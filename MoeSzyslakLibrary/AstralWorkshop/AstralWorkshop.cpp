#include "..\pch.h"
#include "AstralWorkshop.h"
#include "..\VLString.h"

Quest::Quest(const wchar_t* szName, int nDuration, int nDifficulty)
{
	m_name = szName;
	m_duration = nDuration;
	m_difficulty = nDifficulty;
}

Quest::~Quest()
{

}

void Quest::Copy(Quest& copyFrom)
{
	m_name = copyFrom.m_name;
	m_duration = copyFrom.m_duration;
	m_difficulty = copyFrom.m_difficulty;

}

void Quest::Randomize()
{
	Dice d(4, 20260529);
	int rl = d.Roll();

	rl -= 2;
	m_duration += rl;
	rl = d.Roll();

	rl -= 2;
	m_difficulty += rl;
}



Character::Character(const wchar_t* szName) : Creature(szName)
{
	m_bComplete = false;
	m_activity = ACTIVITY_IDLE;
	m_background = BACKGROUND_NONE;
	m_nEditStep = 0;

}

Character::~Character()
{

}

HRESULT Character::CreateCharacter(wstring strCmd, wstring& strOut)
{
	int n = 0;

	switch (m_nEditStep)
	{
	case 0 :
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



AstralWorkshop::AstralWorkshop()
{
	m_cRef = 1;
	m_state = 0;
	m_pArea = new Area(L"The Whispering Glade", 0);
	m_pNewCharacter = NULL;
}

AstralWorkshop::~AstralWorkshop()
{
	printf("ComObjectBase destructor called\n");	
	m_pArea->Release();
	delete m_pNewCharacter;
}

HRESULT __stdcall AstralWorkshop::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == ASTRALWORKSHOP_IID)
	{
		*ppvObj = static_cast<IASTRALWORKSHOP*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall AstralWorkshop::AddRef()
{
	ULONG newCount = InterlockedIncrement(&m_cRef);
	printf("AddRef -> %lu\n", newCount);
	return m_cRef;
}

ULONG __stdcall AstralWorkshop::Release()
{
	ULONG newCount = InterlockedDecrement(&m_cRef);
	
	printf("Release -> %lu\n", newCount);

	if (newCount == 0)
	{
		printf("Refcount reached zero, deleting object\n");
	}

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


HRESULT __stdcall AstralWorkshop::Command(const wchar_t* szCmd, wchar_t* szRet, UINT nLen)
{
	VLStringCollection wrds;
	wstring strOut;

	if (szCmd == NULL || szRet == NULL)
		return E_FAIL;

	if (m_pNewCharacter != NULL && !m_pNewCharacter->IsComplete())
	{
		m_pNewCharacter->CreateCharacter(szCmd, strOut);
		

		if (m_pNewCharacter->IsComplete())
		{
			m_pNewCharacter->SetArea(m_pArea);
			strOut += L"\n" + m_pNewCharacter->GetName() + L" is now in " + m_pArea->GetName();
		}

		wcsncpy_s(szRet, nLen, strOut.c_str(), _TRUNCATE);

		return S_OK;
	}

	wrds.Split(szCmd, ' ');
	wrds.ToLower(0);

	if (wcslen(szCmd) == 0)
	{
		wcsncpy_s(szRet, nLen, L"Enter a command:", _TRUNCATE);
		return S_OK;
	}
		

	switch (szCmd[0])
	{
	case L'c':
	case L'C':
		if (wrds.Compare(0, L"create"))
		{
			delete m_pNewCharacter;
			m_pNewCharacter = new Character(L"");
			m_pNewCharacter->CreateCharacter(L"", strOut);

			wcsncpy_s(szRet, nLen, strOut.c_str(), _TRUNCATE);
			return S_OK;
		}
		break;

	case L'e':
	case L'E':
		if (wrds.Compare(0, L"exit"))
		{
			wcsncpy_s(szRet, nLen, L"Exiting Astral Workshop", _TRUNCATE);
			return S_OK;
		}
		break;
	}

	wcsncpy_s(szRet, nLen, L"I don't know what to do with that command.  Enter Command:", _TRUNCATE);
	return S_OK;
}

HRESULT __stdcall AstralWorkshop::UnitTest()
{
	Character* pChar = new Character(L"");

	Location& forestEdge = m_pArea->AddLocation();
	Location& oldStoneWell = m_pArea->AddLocation();

	
	HRESULT hr = pChar->UnitTest();
	

	wchar_t szLines[] = L"The wind carries the scent of pine and distant rain.\n"
		L"In the valley below, lanterns flicker to life as dusk settles.\n"
		L"Travelers whisper of unrest in the old kingdoms.\n"
		L"And somewhere, your story quietly begins...\n";

	wprintf(L"%s\n", szLines);

	
	
	delete pChar;


	return hr;
}



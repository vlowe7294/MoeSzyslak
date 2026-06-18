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






AstralWorkshop::AstralWorkshop()
{
	m_cRef = 1;
	m_state = STATE_NORMAL;
	m_pStartArea = NULL;

	m_pNewCharacter = NULL;
}

AstralWorkshop::~AstralWorkshop()
{
	printf("ComObjectBase destructor called\n");	

	if (m_pNewCharacter != NULL)
		m_pNewCharacter->Release();

	if (m_pStartArea != NULL)
		m_pStartArea->Release();
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

	if (wcslen(szCmd) == 0)
	{
		wcsncpy_s(szRet, nLen, L"Enter a command:", _TRUNCATE);
		return S_OK;
	}

	wrds.Split(szCmd, ' ');
	wrds.ToLower(0);

	if (m_state == STATE_CHARACTER_CREATION)
	{
		HandleCharacterCreationCommand(szCmd, szRet, nLen);
		return S_OK;
	}

	if (m_state == STATE_WORLD_EDITOR)
	{
		wrds.ToLower(1);
		if (wrds.Compare(0, L"new"))
		{
			if (wrds.Compare(1, L"area"))
			{

				swprintf_s(szRet, nLen, L"Created area '%s' (danger %s).", wrds.Get(2).c_str(), wrds.Get(3).c_str());

			}
			else
				wcsncpy_s(szRet, nLen, L"I don't know what that is.  Enter Command:", _TRUNCATE);

		}
		else
			wcsncpy_s(szRet, nLen, L"I don't know what to do with that command.  Enter Command:", _TRUNCATE);

		return S_OK;
	}

		

	switch (szCmd[0])
	{
	case L'e':
	case L'E':
		if (wrds.Compare(0, L"exit"))
		{
			wcsncpy_s(szRet, nLen, L"Exiting Astral Workshop", _TRUNCATE);
			return S_OK;
		}
		else if (wrds.Compare(0, L"editor"))
		{
			wcsncpy_s(szRet, nLen, L"World editor activated", _TRUNCATE);
			m_state = STATE_WORLD_EDITOR;
			return S_OK;
		}
		break;
	}

	wcsncpy_s(szRet, nLen, L"I don't know what to do with that command.  Enter Command:", _TRUNCATE);
	return S_OK;
}

HRESULT __stdcall AstralWorkshop::NewArea(IUnknown** iArea, int* ndx, const wchar_t* szName, int nDanger)
{
	m_areas.Count(ndx);
	Area* pNewArea = new Area(szName, nDanger);

	pNewArea->QueryInterface(IID_IUnknown, (void**)iArea);
	m_areas.Add(pNewArea, szName, 0);
	pNewArea->Release();
	return S_OK;
}

void AstralWorkshop::HandleCharacterCreationCommand(const wchar_t* szCmd, wchar_t* szRet, UINT nLen)
{
	wstring strOut;
	m_pNewCharacter->CreateCharacter(szCmd, strOut);

	if (m_pNewCharacter->IsComplete())
	{
		m_pNewCharacter->SetArea(m_pStartArea);
		strOut += L"\n" + m_pNewCharacter->GetName() + L" is now in " + m_pStartArea->GetName();
		m_state = STATE_NORMAL;
	}

	wcsncpy_s(szRet, nLen, strOut.c_str(), _TRUNCATE);		
}

HRESULT __stdcall AstralWorkshop::Export(const wchar_t* szPath)
{
	Database* pDB = new Database();
	Area* pArea = NULL;
	Table* iTbl = NULL;
	int r = 0;

	pDB->GetTable(L"areas", (IUnknown**)&iTbl);

	while (m_areas.ForEach((IUnknown**)&pArea) == S_OK)
	{
		if (r > 0)
			iTbl->NewRow();
		r++;

		pArea->Save(*iTbl);
	}

	VLFile fle;
	fle.Create(szPath);

	if (!fle.IsOpen())
		return E_FAIL;

	fle.Write(pDB->Export());
	pDB->Release();
	return S_OK;
}

HRESULT __stdcall AstralWorkshop::Save(const wchar_t* szPath)
{
	Database* pDB = new Database();
	Area* pArea = NULL;
	Table* iTbl = NULL;
	int r = 0;

	pDB->GetTable(L"areas", (IUnknown**)&iTbl);

	while (m_areas.ForEach((IUnknown**)&pArea) == S_OK)
	{
		if (r > 0)
			iTbl->NewRow();
		r++;
		pArea->Save(*iTbl);
	}

	pDB->Save(szPath);
	pDB->Release();
	return S_OK;

}

HRESULT __stdcall AstralWorkshop::NewCharacter(IUnknown** iCharacter, const wchar_t* szName, int nClass, int nBackground)
{
	if (m_pNewCharacter != NULL)
		m_pNewCharacter->Release();

	if (nClass < 0 || nClass >= Character::CLASS_MAX)
		nClass = Character::CLASS_NONE;

	if (nBackground < 0 || nBackground >= Character::BACKGROUND_MAX)
		nBackground = Character::BACKGROUND_NONE;

	m_pNewCharacter = new Character(szName, (Creature::CLASS)nClass, (Character::BACKGROUND)nBackground);
	m_pNewCharacter->QueryInterface(IID_IUnknown, (void**)iCharacter);
	return S_OK;
}

HRESULT __stdcall AstralWorkshop::UnitTest()
{
	Character* pChar = NULL;
	NewCharacter((IUnknown**)&pChar, L"Gert Addams", 1, 2);
	pChar->Release();
	return S_OK;

}



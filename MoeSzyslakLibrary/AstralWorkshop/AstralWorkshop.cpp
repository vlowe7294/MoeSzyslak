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
	g_memoryChecker.IncrementInstance(CLASSID::ASTRALWORKSHOP);
	m_pStartArea = NULL;

	m_pNewCharacter = NULL;
	m_pTester = new Testing();
}

AstralWorkshop::~AstralWorkshop()
{
	g_memoryChecker.DecrementInstance(CLASSID::ASTRALWORKSHOP);

	if (m_pNewCharacter != NULL)
		m_pNewCharacter->Release();

	if (m_pStartArea != NULL)
		m_pStartArea->Release();

	m_pTester->Release();
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
	return m_cRef;
}

ULONG __stdcall AstralWorkshop::Release()
{
	InterlockedDecrement(&m_cRef);

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


HRESULT __stdcall AstralWorkshop::NewArea(IUnknown** iArea, int* ndx, const wchar_t* szName, int nDanger)
{
	*ndx = m_areas.Count();
	Area* pNewArea = new Area(szName, nDanger);

	if (m_pStartArea == NULL)
	{
		m_pStartArea = pNewArea;
		m_pStartArea->AddRef();
	}

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

	while (m_areas.ForEach((IUnknown**)&pArea))
	{
		if (r > 0)
			iTbl->NewRow();
		r++;

		if (pArea != NULL)
		{
			pArea->Save(*iTbl);
			pArea->Release();
		}
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

	while (m_areas.ForEach((IUnknown**)&pArea))
	{
		if (r > 0)
			iTbl->NewRow();
		r++;

		if (pArea != NULL)
		{
			pArea->Save(*iTbl);
			pArea->Release();
		}		
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

HRESULT __stdcall AstralWorkshop::Heartbeat(int nSec)
{
	printf("AstralWorkshop::Heartbeat line 201\n");
	m_worldTimeSec += nSec;
	Area* iArea = NULL;

	while (m_areas.ForEach((IUnknown**)&iArea))
	{
		if (iArea != NULL)
		{
			iArea->Tick(nSec);
			iArea->Release();
		}
	}

	return S_OK;
}

HRESULT __stdcall AstralWorkshop::GetTester(IUnknown** iTst)
{
	m_pTester->QueryInterface(IID_IUnknown, (void**)iTst);
	return S_OK;
}

HRESULT __stdcall AstralWorkshop::GetAreaNames(BSTR* areaList)
{
	Area* iArea = NULL;
	wstring list;

	
	while (m_areas.ForEach((IUnknown**)&iArea))
	{
		if (iArea != NULL)
		{
			if (!list.empty())
				list += L"\n";

			list += iArea->GetName();
			iArea->Release();
		}
	}

	*areaList = SysAllocString(list.c_str());
	return S_OK;
}



HRESULT __stdcall AstralWorkshop::UnitTest()
{
	wstring gameCommand = L"new character", result;

	// Trim whitespace
	if (gameCommand.size() == 0)
	{
		result = L"Enter a command.";
		return S_OK;
	}

	VLStringCollection wrds;
	wrds.Split(gameCommand.c_str(), ' ');
	wrds.ToLower(0);
	std::wstring verb = wrds.Get(0);

	// ---------------------------------------------------------
	// 1. Character Creation Required Before Gameplay
	// ---------------------------------------------------------
	if (m_pNewCharacter == nullptr)
	{
		// Only allow "create" or "new" commands
		if (verb == L"create" || verb == L"new")
		{
			if (wrds.GetCount() < 2)
			{
				result = L"Usage: create <name>";
			}
			else
			{
				std::wstring name = wrds.Get(0);
				m_pNewCharacter = new Character(name.c_str(), Creature::CLASS_WARRIOR, Character::BACKGROUND_PEASANT);
				result = L"Character '" + name + L"' created.\n"
					L"You may now begin your adventure.";
			}
		}
		else
		{
			result = L"No character exists.\n"
				L"Create one using: create <name>";
		}

	}

	

	
	result = L"I don't understand that command.";

	wprintf(L"%s\n%s  Enter command:  \n", gameCommand.c_str(), result.c_str());
	
	return S_OK;
}



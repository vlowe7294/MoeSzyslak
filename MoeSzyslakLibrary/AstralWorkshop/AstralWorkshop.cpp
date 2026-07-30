#include "..\pch.h"
#include "AstralWorkshop.h"
#include "..\VLString.h"

Quest::Quest(const wchar_t* szName, int nDuration, int nDifficulty)
{
	m_cRef = 1;
	m_name = szName;
	m_duration = nDuration;
	m_difficulty = nDifficulty;
}

Quest::~Quest()
{

}

HRESULT __stdcall Quest::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Quest::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Quest::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
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


HRESULT __stdcall AstralWorkshop::NewArea(IUnknown** iArea, int* ndx, int nDanger)
{
	*ndx = m_areas.Count();
	Area* pNewArea = new Area();

	if (m_pStartArea == NULL)
	{
		m_pStartArea = pNewArea;
		m_pStartArea->AddRef();
	}

	pNewArea->QueryInterface(IID_IUnknown, (void**)iArea);
	m_areas.Add(pNewArea, L"", 0);
	pNewArea->Release();
	return S_OK;
}

void AstralWorkshop::HandleCharacterCreationCommand(const wchar_t* szCmd, wchar_t* szRet, UINT nLen)
{
	wstring strOut;
	m_pNewCharacter->CreateCharacter(szCmd, strOut);
	VariableCollection* iVar = NULL;
	
	if (m_pNewCharacter->IsComplete())
	{
		m_pNewCharacter->SetArea(m_pStartArea);
		m_pStartArea->GetProperties((IUnknown**)&iVar);

		wstring nme = iVar->Get(L"Name");
		strOut += L"\n" + m_pNewCharacter->GetName() + L" is now in " + nme;
		iVar->Release();

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
	VariableCollection* iVar = NULL;

	
	while (m_areas.ForEach((IUnknown**)&iArea))
	{
		if (iArea != NULL)
		{
			if (!list.empty())
				list += L"\n";

			iVar = NULL;
			iArea->GetProperties((IUnknown**)&iVar);

			list += iVar->Get(L"Name");
			iVar->Release();
			iArea->Release();
		}
	}

	*areaList = SysAllocString(list.c_str());
	return S_OK;
}



HRESULT __stdcall AstralWorkshop::UnitTest()
{
	
	return S_OK;
}



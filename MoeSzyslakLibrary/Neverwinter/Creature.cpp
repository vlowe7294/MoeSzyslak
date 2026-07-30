#include "pch.h"
#include "Creature.h"
#include "Placeable.h"

Creature::Creature(const wchar_t* szName)
{
	m_cRef = 1;
	m_name = szName;
	m_class = CLASS_NONE;

	m_thirst.amt = 0;
	m_thirst.count = 0;
	m_thirst.rate = 1700;

	memset(m_pConnectedObjects, 0, sizeof(void*) * 10);
	memset(m_nTravelTimes, 0, sizeof(int) * 10);
	m_faction = FACTION_EXPLORER;
	m_pMoveTowards = NULL;

	m_hunger.amt = 0;
	m_hunger.count = 0;
	m_hunger.rate = 1700;
	m_courage = 10;
	m_pArea = NULL;
	m_nAlignment[0] = m_nAlignment[1] = m_nAlignment[2] = m_nAlignment[3] = 50;
}

Creature::~Creature()
{
	if (m_pArea != NULL)
		m_pArea->Release();
}

HRESULT __stdcall Creature::QueryInterface(REFIID riid, LPVOID* ppvObj)
{

	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == CREATURE_IID)
	{
		*ppvObj = static_cast<ICREATURE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Creature::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Creature::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Creature::Tick(int nSec)
{
	m_hunger.count += nSec;
	m_thirst.count += nSec;
	
	while (m_hunger.count >= m_hunger.rate)
	{
		m_hunger.amt++;
		m_hunger.count -= m_hunger.rate;
	}

	while (m_thirst.count >= m_thirst.rate)
	{
		m_thirst.amt++;
		m_thirst.count -= m_thirst.rate;
	}

	if (m_faction == FACTION_EXPLORER && m_pMoveTowards == NULL)
	{
		m_pMoveTowards = m_pConnectedObjects[0];
		m_nMoveTime = m_nTravelTimes[0];
	}

	if (m_nMoveTime > 0)
	{
		m_nMoveTime -= nSec;

		if (m_nMoveTime <= 0)
		{
			m_pMoveTowards = NULL;
			memset(m_pConnectedObjects, 0, sizeof(void*) * 10);
			memset(m_nTravelTimes, 0, sizeof(int) * 10);			
		}
	}

	return S_OK;
}

HRESULT __stdcall Creature::SetArea(IUnknown* iUnk)
{
	IAREA* iArea = NULL;

	if (m_pArea != NULL)
	{
		m_pArea->Release();
		m_pArea = NULL;
	}

	// make sure it's a legitimate area object

	if (iUnk == NULL)
		return E_FAIL;

	iUnk->QueryInterface(AREA_IID, (void**)&iArea);

	if (iArea == NULL)
		return E_FAIL;

	m_pArea = iUnk;
	return S_OK;
}

HRESULT __stdcall Creature::GetIsPC(BOOL* bIsPC)
{
	*bIsPC = FALSE;
	return S_OK;
}

void Creature::AddConnectedObject(void* pObject, int nTravelSec)
{
	int i = 0;

	while (m_pConnectedObjects[i] != NULL && i < 10)
		i++;

	if (i < 10)
	{
		m_pConnectedObjects[i] = pObject;
		m_nTravelTimes[i] = nTravelSec;
	}

}

void Creature::AdjustAlignment(int nAlignment, int nShift)
{
	int opposeNdx = 0;

	switch (nAlignment)
	{
	case ALIGNMENT_LAWFUL:
		opposeNdx = ALIGNMENT_CHAOTIC;
		break;

	case ALIGNMENT_CHAOTIC:
		opposeNdx = ALIGNMENT_LAWFUL;
		break;

	case ALIGNMENT_GOOD:
		opposeNdx = ALIGNMENT_EVIL;
		break;

	case ALIGNMENT_EVIL:
		opposeNdx = ALIGNMENT_GOOD;
		break;

	default:  // bad index
		return;
		break;
	}

	m_nAlignment[nAlignment] += nShift;
	m_nAlignment[opposeNdx] -= nShift;

	if (m_nAlignment[nAlignment] < 0)
		m_nAlignment[nAlignment] = 0;

	if (m_nAlignment[opposeNdx] < 0)
		m_nAlignment[opposeNdx] = 0;

	if (m_nAlignment[nAlignment] > 100)
		m_nAlignment[nAlignment] = 100;

	if (m_nAlignment[opposeNdx] > 100)
		m_nAlignment[opposeNdx] = 100;

}





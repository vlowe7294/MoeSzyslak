#include "pch.h"
#include "Creature.h"
#include "Placeable.h"

Creature::Creature()
{
	m_cRef = 1;
	m_thirst = 0;
	m_thirstRate = 1700;
	m_nCount = 0;
	memset(&m_location, 0, sizeof(LOCATION));
	memset(m_pConnectedObjects, 0, sizeof(void*) * 10);
	memset(m_nTravelTimes, 0, sizeof(int) * 10);
	m_faction = FACTION_EXPLORER;
	m_pMoveTowards = NULL;

}

Creature::~Creature()
{

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
	m_nCount += nSec;

	if (m_nCount >= m_thirstRate)
	{
		m_thirst++;
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
			memset(&m_location, 0, sizeof(LOCATION));
			memset(m_pConnectedObjects, 0, sizeof(void*) * 10);
			memset(m_nTravelTimes, 0, sizeof(int) * 10);			
		}
	}

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
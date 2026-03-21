#include "pch.h"
#include "Placeable.h"

Placeable::Placeable()
{
	m_cRef = 1;
	m_nWater = 0;
	m_nWaterCapacity = 100;
	m_regenRate = 240;
	m_nSecCount = 0;

}

Placeable::~Placeable()
{

}

HRESULT __stdcall Placeable::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall Placeable::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Placeable::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

void Placeable::Tick(int nSec)
{
	m_nSecCount += nSec;

	if (m_nSecCount >= m_regenRate)
	{
		if (m_regenRate > 0)
		{
			m_nWater = m_nWaterCapacity;
			m_nSecCount = m_nSecCount - m_regenRate;
		}
		else
			m_nSecCount = 0;
	}
}
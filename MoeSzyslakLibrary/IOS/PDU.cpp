#include "pch.h"
#include "PDU.h"
#include "..\VLVariable.h"

PDU::PDU(UINT id)
{
	m_cRef = 1;
	m_id = id;
}

PDU::~PDU()
{

}

ULONG PDU::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall PDU::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall PDU::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

void PDU::Value(int nVal)
{
	m_iVariable->SetAsInt(nVal, 0);
}

void PDU::Value(double fVal)
{
	m_iVariable->SetAsFloat(0, fVal);

}

void PDU::Copy(PDU& p)
{
	BOOL bChanged = FALSE;
	m_id = p.m_id;
	m_iVariable->Copy(p.m_iVariable);
	VLVariable* pVar = (VLVariable*)((IUnknown*)m_iVariable);
	m_iVariable->HasChanged(&bChanged);

	if (bChanged == TRUE)
	{
		m_timeStamp.SetToNow();
		pVar->TurnOffChanged();
	}
}

int PDU::GetIntValue()
{
	int nRet = 0;
	m_iVariable->GetAsInt(0, &nRet);
	return nRet;
}

double PDU::GetFloatValue()
{
	double fRet = 0.0;
	m_iVariable->GetAsFloat(0, &fRet);
	return fRet;

}

bool PDU::Compare(PDU& p)
{
	int nCmp = 0;

	m_iVariable->Compare(p.m_iVariable, &nCmp);

	return nCmp == 0;
}

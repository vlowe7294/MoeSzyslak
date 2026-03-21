#pragma once

#include "..\VLDateTime.h"

class PDU : public IUnknown
{
public:

	PDU(UINT id);
	~PDU();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void Value(int nVal);
	void Value(double fVal);
	void Copy(PDU& p);
	int GetIntValue();
	double GetFloatValue();
	bool Compare(PDU& p);

	inline UINT ID() { return m_id; }

private:
	int m_cRef;
	UINT m_id;
	VariableInf m_iVariable;
	VLDateTime m_timeStamp;

};

#pragma once

#include "InterfaceCollection.h"
#include "Interfaces.h"

class VLDateTime : public IDATETIME
{
public:
	VLDateTime(VLDateTime& dt);
	VLDateTime();
	VLDateTime(int yr, int mnth, int dy);
	~VLDateTime();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall SetDay(UINT yr, UINT mnth, UINT dy);
	HRESULT __stdcall SetTime(UINT hr, UINT min, UINT sec);
	HRESULT __stdcall GetDay(UINT* yr, UINT* mnth, UINT* dy);
	HRESULT __stdcall GetTime(UINT* hr, UINT* min, UINT* sec);
	HRESULT __stdcall GetDifference(IUnknown* iSubAmt, int* nMin);
	HRESULT __stdcall ToString(IUnknown* iStr);
	HRESULT __stdcall TimeFormat(IUnknown* iStr);
	HRESULT __stdcall Copy(IUnknown* iCpyFrom);


	HRESULT __stdcall Get(IUnknown* iDta);
	HRESULT __stdcall ReadFromXML(LPCWSTR szPath) { return E_FAIL; };

	void AddSeconds(double nSec);
	void AddMinutes(int nMin);
	void AddDays(int nDays);
	void AddMonths(int nMonths);

	void Print();
	void Copy(VLDateTime& tm);
	void Parse(std::wstring inStr);
	bool Compare(VLDateTime& dt);
	void SetToNow();

	bool operator > (VLDateTime& dt);
	bool operator >= (VLDateTime& dt);

	inline int GetYear() { return m_data[0]; };
	inline int GetMonth() { return m_data[1]; };
	inline int GetDay() { return m_data[2]; };
	inline int GetMinutes() { return m_nMin % 60; };
	inline int GetHours() { return m_nMin / 60; };
	inline int GetSeconds() { return (int)m_nSec; };
	inline time_t GetTotalSeconds() { return m_timeSec; };


private:
	int m_cRef;
	int m_data[3];
	int m_nMin;
	double m_nSec;
	time_t m_timeSec;

	void SetTimeStruct();
	void GetFromTimeStruct();
};

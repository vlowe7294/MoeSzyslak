#pragma once
#include <chrono>
#include "Interfaces.h"
#include "VLString.h"
#include "Utilities/Database.h"

class LogEntry : public ILOGENTRY
{
public:
	LogEntry(wstring txt, long long tme, long long mem);
	~LogEntry();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Properties(IUnknown** iProp);
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);

	void Write(IUnknown* iTbl);
	void Read(IUnknown* iTbl);

private:
	int m_cRef;
	VariableCollection* m_pProperties;
	VLVariable* m_pTxt;
	VLVariable* m_pTime;
	VLVariable* m_pMemUsed;
	VLVariable* m_pReturnVar;
};

class Testing : public TestingInf::ITESTING
{
public:

	Testing();
	~Testing();

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall RunTest(UINT nClassID);
	HRESULT __stdcall VerifyVariable(LPCWSTR varName, LPCWSTR val);
	HRESULT __stdcall VerifyHResult(HRESULT hr, LPCWSTR szMsg);
	HRESULT __stdcall Message(LPCWSTR szMsg);
	HRESULT __stdcall Save(LPCWSTR szFilePath);
	HRESULT __stdcall Load(LPCWSTR szFilePath);
	HRESULT __stdcall GetTestData(LPCWSTR szName, IUnknown* iStr);
	HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal);
	

	HRESULT __stdcall UnitTest();

private:
	int m_cRef;
	
	VLVariable* m_pPass;
	VLVariable* m_pMemCheck;
	VLVariable* m_pReportVar;
	VLVariable* m_pLogEntriesVar;
	VariableCollection* m_pProperties;

	std::chrono::steady_clock::time_point m_timeStamp;
	MEMORYSTATUSEX m_memInfo;
	InterfaceCollectionInf m_iEntries;
	VLDateTime* m_pTestTime;
	StringInf m_iRetStr;
	VariableCollection* m_pTestValues;
	
	void VariableTest();
	void TripPlannerTest();
	void NeverwinterTest();
	void IOSTest();
	void UserTest();
	void FinanceTest();
	void SelfTest();

	void Verify(bool bVal, LPCWSTR szMsg);	
	wstring GetTestData(LPCWSTR szVarName);	
	void Report();	
};

#pragma once

class TestingInf
{
public:
	static const UINT ClassID = 398981;

	// {7C6DA0F8-84AE-4E97-86F0-13BB1E67C713}
	static const GUID m_iid;

	struct ITESTING : public IUnknown
	{
		virtual HRESULT __stdcall Message(LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory) = 0;
		virtual HRESULT __stdcall GetLogEntry(int ndx, IUnknown** iEntry) = 0;
		virtual HRESULT __stdcall VerifyVariable(LPCWSTR varName, LPCWSTR val) = 0;
		virtual HRESULT __stdcall VerifyHResult(HRESULT hr, LPCWSTR szMsg) = 0;		
		virtual HRESULT __stdcall GetTestData(LPCWSTR szName, BSTR* iStr) = 0;
		virtual HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal) = 0;
		virtual HRESULT __stdcall GetClassName(UINT nClassID, BSTR* bbsClassName) = 0;
		virtual HRESULT __stdcall GetClassID(LPCWSTR szClassName, UINT* nClassID) = 0;
		virtual HRESULT __stdcall Report() = 0;
		virtual HRESULT __stdcall Verify(BOOL bVal, LPCWSTR szMsg, int nDebugLvl, LPCWSTR szCategory) = 0;
		virtual HRESULT __stdcall NewEntry(IUnknown** iEntry) = 0;
		virtual HRESULT __stdcall GetProperties(IUnknown** iPrp) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	TestingInf();
	~TestingInf();
	bool Attach(IUnknown* iunk);
	ITESTING* operator->();
	operator IUnknown* ();

private:
	IUnknown* m_iUnk;
	ITESTING* m_iVar;

	void Attach();
};


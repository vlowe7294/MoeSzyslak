#pragma once

class TestingInf
{
public:
	static const UINT ClassID = 398981;

	// {7C6DA0F8-84AE-4E97-86F0-13BB1E67C713}
	static const GUID m_iid;

	struct ITESTING : public IUnknown
	{
		virtual HRESULT __stdcall Properties(IUnknown** iPrp) = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall RunTest(UINT nClassID) = 0;
		virtual HRESULT __stdcall VerifyVariable(LPCWSTR varName, LPCWSTR val) = 0;
		virtual HRESULT __stdcall VerifyHResult(HRESULT hr, LPCWSTR szMsg) = 0;
		virtual HRESULT __stdcall Message(LPCWSTR szMsg) = 0;
		virtual HRESULT __stdcall GetTestData(LPCWSTR szName, IUnknown* iStr) = 0;
		virtual HRESULT __stdcall SetTestData(LPCWSTR szName, LPCWSTR szVal) = 0;
		virtual HRESULT __stdcall GetClassName(UINT nClassID, IUnknown* iStrClassName) = 0;
		virtual HRESULT __stdcall GetClassID(LPCWSTR szClassName, UINT* nClassID) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	TestingInf();
	~TestingInf();
	bool Attach(IUnknown* iunk);
	ITESTING* operator->();

private:
	IUnknown* m_iUnk;
	ITESTING* m_iVar;

	void Attach();
};


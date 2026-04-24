#pragma once

class FinanceInf
{
public:
	struct IFINANCE : public IUnknown
	{
		virtual HRESULT __stdcall HTMLPage(const wchar_t* szVarList, IUnknown* iHtml) = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	static const UINT ClassID = 346141;

	// {3BAD86FA-DEC6-42C7-955A-2069B637BFC9}
	static GUID m_iid;

	FinanceInf();
	~FinanceInf();
	void Attach(IUnknown* iunk = NULL);

	IFINANCE* operator->();
	inline operator IUnknown* () { return m_iUnk; };

private:
	IUnknown* m_iUnk;
	IFINANCE* m_iVar;

	void Init();
	void GetInterface();
};

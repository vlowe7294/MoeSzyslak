#pragma once

class NeverwinterInf
{
public:
	struct INEVERWINTER : public IUnknown
	{
		virtual HRESULT __stdcall UnitTest() = 0;
		virtual HRESULT __stdcall NWNExit() = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall Properties(IUnknown** iProp) = 0;
		virtual HRESULT __stdcall Dispose() = 0;
	};

	static const UINT ClassID = 977093;
	NeverwinterInf();
	~NeverwinterInf();
	bool Attach(IUnknown* iunk);
	void Attach();
	bool Dispose();

	INEVERWINTER* operator->();
	inline operator IUnknown* () { return m_iUnk; };

private:
	IUnknown* m_iUnk;
	INEVERWINTER* m_iVar;

};

#pragma once

class IOSInf
{
public:
	static const UINT ClassID = 306758;

	struct IIOS : public IUnknown
	{
		virtual HRESULT __stdcall Properties(IUnknown** iProp) = 0;		
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall Update() = 0;
		virtual HRESULT __stdcall EstablishExercise() = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
	};

	IOSInf();
	~IOSInf();
	IIOS* operator->();
	bool Attach(IUnknown* iunk);

	inline operator IUnknown* () { return m_iUnk; };

private:
	IUnknown* m_iUnk;
	IIOS* m_iVar;
};

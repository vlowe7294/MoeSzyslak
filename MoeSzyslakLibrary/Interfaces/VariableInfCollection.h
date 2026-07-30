#pragma once

class VariableInf
{
public:
	static const UINT ClassID = 87153;

	// {D383C87B-63DD-4875-BC77-74B01F11226B}
	static GUID m_iid;

	/**
	 * @interface IVARIABLE
	 * @brief Represents a flexible, COM‑exposed variable supporting multiple data types,
	 *        change tracking, limits, serialization, and interface storage.
	 */

	struct IVARIABLE : public IUnknown
	{
		/** @brief Retrieves the variable as a BOOL. */
		virtual HRESULT __stdcall GetAsBool(BOOL* bVal) = 0;


		virtual HRESULT __stdcall SetAsBool(BOOL bVal) = 0;
		virtual HRESULT __stdcall Command(const wchar_t* szCmd) = 0;
		virtual HRESULT __stdcall GetReturnString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall GetType(UINT* nType) = 0;
		virtual HRESULT __stdcall SetType(UINT nType) = 0;
		virtual HRESULT __stdcall GetDisplayName(IUnknown** iNmeStr) = 0;
		virtual HRESULT __stdcall UnitTest() = 0;
		virtual HRESULT __stdcall GetString(IUnknown** iStr) = 0;
		virtual HRESULT __stdcall SetString(const wchar_t* szStr) = 0;
		virtual HRESULT __stdcall SetAsInt(int nVal, int ndx) = 0;
		virtual HRESULT __stdcall GetAsInt(int ndx, int* nVal) = 0;
		virtual HRESULT __stdcall SetAsFloat(int ndx, double fVal) = 0;
		virtual HRESULT __stdcall GetAsFloat(int ndx, double* fVal) = 0;
		virtual HRESULT __stdcall SetInterface(IUnknown* iunk, UINT nClassID) = 0;
		virtual HRESULT __stdcall GetInterface(IUnknown** iunk) = 0;
		virtual HRESULT __stdcall GetLocked(BOOL* bLocked) = 0;
		virtual HRESULT __stdcall SetLocked(BOOL bLocked) = 0;
		virtual HRESULT __stdcall Save(IUnknown* iTbl) = 0;
		virtual HRESULT __stdcall Copy(IUnknown* iVar) = 0;
		virtual HRESULT __stdcall HasChanged(BOOL* bChanged) = 0;
		virtual HRESULT __stdcall Compare(IUnknown* iVar, int* nCmp) = 0;
	};

	VariableInf();
	~VariableInf();
	void Attach(IUnknown* iunk);
	void Init();
	bool Verify();
	void Dispose();
	const wchar_t* GetString();
	void SetString(const wchar_t* szStr);
	void SetInt(int nVal, int ndx);

	IVARIABLE* operator->();
	inline operator IUnknown** () { return &m_iUnk; };
	inline operator IUnknown* () { return m_iUnk; };
	inline operator IVARIABLE* () { return m_iVar; };

private:
	IUnknown* m_iUnk;
	IVARIABLE* m_iVar;
	StringInf m_str;

	void GetInterface();
};

class VariableInfCollection
{
public:
	VariableInfCollection(IUnknown* iunk = NULL);
	~VariableInfCollection();
	std::wstring Get(std::wstring nme);
	void GetVariableInterface(std::wstring nme, IUnknown** iunk);
	void Set(std::wstring nme, std::wstring val);	
	bool Edit(std::wstring& prmpt, std::wstring val);
	void Attach();
	bool IsEmpty();
	bool GetBool(std::wstring nme);
	void SetBool(std::wstring nme, bool bVal);
	bool ForEach(IUnknown** iunk);


	InterfaceCollectionInf::IINTERFACECOLLECTION* operator->();

	inline operator IUnknown** () { return &m_iUnk; };
	inline operator IUnknown* () { return m_iUnk; };

private:
	IUnknown* m_iUnk;
	IUnknown* m_iLastVariable;
	InterfaceCollectionInf m_iCollection;

	void Get(std::wstring nme, IUnknown** iVar);
};

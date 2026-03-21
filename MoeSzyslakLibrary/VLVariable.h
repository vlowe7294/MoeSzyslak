#pragma once

#include "InterfaceCollection.h"
#include "VLString.h"
#include "VLDateTime.h"
#include "Utilities/VLFile.h"
class VLVariable : public VariableInf::IVARIABLE
{
public:
	enum VAR_TYPE
	{
		TYPE_STRING,
		TYPE_DATE,
		TYPE_INT,
		TYPE_BOOL,
		TYPE_INTERFACE,
		TYPE_FLOAT,
		TYPE_COLLECTION,
		TYPE_MAX
	};

	VLVariable();
	~VLVariable();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall GetAsBool(BOOL* bVal);
	HRESULT __stdcall SetAsBool(BOOL bVal);
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall GetType(UINT* nType);
	HRESULT __stdcall SetType(UINT nType);
	HRESULT __stdcall GetDisplayName(IUnknown** iNmeStr);
	HRESULT __stdcall UnitTest();
	HRESULT __stdcall GetString(IUnknown** iStr);
	HRESULT __stdcall SetString(const wchar_t* szStr);	
	HRESULT __stdcall SetAsInt(int nVal, int ndx);
	HRESULT __stdcall GetAsInt(int ndx, int* nVal);
	HRESULT __stdcall SetAsFloat(int ndx, double fVal);
	HRESULT __stdcall GetAsFloat(int ndx, double* fVal);
	HRESULT __stdcall SetInterface(IUnknown* iunk, UINT nClassID);
	HRESULT __stdcall GetInterface(IUnknown** iunk);
	HRESULT __stdcall GetLocked(BOOL* bLocked);
	HRESULT __stdcall SetLocked(BOOL bLocked);
	HRESULT __stdcall Save(IUnknown* iTbl);
	HRESULT __stdcall Copy(IUnknown* iVar);
	HRESULT __stdcall HasChanged(BOOL* bChanged);
	HRESULT __stdcall Compare(IUnknown* iVar, int* nCmp);

	void TurnOffChanged();

	inline void CanBeEmpty(bool bCanBeEmpty) { m_bCanBeEmpty = bCanBeEmpty; }

	VLDateTime& GetDateTime();
	void Copy(VLVariable& iCpyFrm);
	void SetLimits(int nMin, int nMax);
	void SaveToFile(VLFile& fle);
	void Load(IUnknown* iTbl);
	void LoadFromFile(VLFile& fle);
	wstring GetString();

private:
	int m_cRef;
	bool m_bLocked;
	StringInf m_iStr;
	StringInf m_iDisplayName;

	double* m_pValues;
	double* m_pOrigValues;
	VAR_TYPE m_type;
	int m_limits[2];
	
	bool m_bCanBeEmpty;
	VLDateTime* m_iDate;
	VLDateTime* m_iOrigDate;
	UINT m_nMaxSize;
	IUnknown* m_iunk;
	IUnknown* m_iReturnUnk;
	StringInf m_iReturnStr;
	wstring m_origValue;
	int m_instance;

	
	void GrowSize(int ndx);
};

class VariableCollection : public InterfaceCollection
{
public:
	VariableCollection();
	~VariableCollection();
	VLVariable* NewVariable(wstring szTag);
	void Set(wstring szTag, wstring szValue, VLVariable::VAR_TYPE type);
	wstring Get(wstring szTag);
	void Save(IUnknown* iTbl);
	void Load(IUnknown* iTbl);
	bool ForEach(VLVariable** pVar);
	VLVariable* Get(int ndx);
	void TurnOffChanged();
	bool HasChanged();
};

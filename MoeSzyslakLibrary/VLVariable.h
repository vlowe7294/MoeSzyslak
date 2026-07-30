#pragma once

#include "InterfaceCollection.h"
#include "VLString.h"
#include "VLDateTime.h"
#include "Utilities/VLFile.h"

/**
 * @class VLVariable
 * @brief Represents a flexible, COM‑exposed variable supporting multiple data types,
 *        change tracking, limits, serialization, and interface storage.
 *
 * This class implements VariableInf::IVARIABLE and provides typed accessors,
 * COM reference counting, persistence helpers, and metadata such as display names.
 */
class VLVariable : public VariableInf::IVARIABLE
{
public:

	/**
     * @enum VAR_TYPE
     * @brief Defines the supported variable types.
     */
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

	/** @brief Retrieves the variable as a BOOL. */
	HRESULT __stdcall GetAsBool(BOOL* bVal);

	/** @brief Sets the variable as a BOOL. */
	HRESULT __stdcall SetAsBool(BOOL bVal);

	/** @brief Executes a command string on the variable. */
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

	VLDateTime& GetDateTime();
	void Copy(VLVariable& iCpyFrm);
	void SetLimits(int nMin, int nMax);
	void SaveToFile(VLFile& fle);
	void Load(IUnknown* iTbl);
	void LoadFromFile(VLFile& fle);
	wstring GetString();
	
	inline void CanBeEmpty(bool bCanBeEmpty) { m_bCanBeEmpty = bCanBeEmpty; }
	inline wstring DisplayName() { return (const wchar_t*)m_iDisplayName; };

private:
	int m_nID;
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
	
	void GrowSize(int ndx);
};

/**
 * @class VariableCollection
 * @brief A COM‑exposed collection of VLVariable objects, accessible by tag or index.
 *
 * Provides typed accessors, iteration, persistence, and change tracking for
 * a dynamic list of variables.
 */
class VariableCollection : public IVARIABLELIST
{
public:
	VariableCollection();
	~VariableCollection();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/** @brief Retrieves a variable's string value by tag. */
	HRESULT __stdcall GetAsString(const wchar_t* szTag, BSTR* bsStr);

	HRESULT __stdcall SetAsString(const wchar_t* szTag, const wchar_t* szVal);

	/** @brief Sets an indexed integer value for a variable by tag. */
	HRESULT __stdcall SetAsInt(const wchar_t* szTag, int nVal, int ndx);

	HRESULT __stdcall GetAsInt(const wchar_t* szTag, int* bVal);
	
	/** @brief Retrieves a boolean value for a variable by tag. */
	HRESULT __stdcall GetAsBool(const wchar_t* szTag, BOOL* bVal);
	
	/** @brief Sets a boolean value for a variable by tag. */
	HRESULT __stdcall SetAsBool(const wchar_t* szTag, BOOL bVal);

	

	/**
	 * @brief Creates a new variable with the given tag. If the variable already exists, variable is replaced with new one,
	 * but the original variable still remains in the list.
	 */
	VLVariable* NewVariable(wstring szTag);

	/** @brief Sets a variable's value and type by tag. */
	void Set(wstring szTag, wstring szValue, VLVariable::VAR_TYPE type);

	/** @brief Retrieves a variable's string value by tag. */
	wstring Get(wstring szTag);
	
	/** @brief Saves all variables to a table object. */
	void Save(IUnknown* iTbl);

	/** @brief Loads all variables from a table object. */
	void Load(IUnknown* iTbl);

	/**
	 * @brief Iterates through variables.
	 * @return true if a variable is returned, false when iteration ends.
	 */
	bool ForEach(VLVariable** pVar);

	/** @brief Retrieves a variable by index. */
	VLVariable* Get(int ndx);

	/** @brief Clears the changed flag on all variables. */
	void TurnOffChanged();
	
	/** @brief Returns whether any variable has changed. */
	bool HasChanged();

	/** @brief Removes all variables. */
	void Clear();

	/** @brief Returns the number of variables. */
	int Count();

	/** @brief Retrieves a variable by tag. */
	VLVariable* GetByTag(const wchar_t* szTag);

private:
	/** @brief Internal COM collection of variables. */
	ComCollection m_variables;

	/** @brief COM reference count. */
	int m_cRef;
};

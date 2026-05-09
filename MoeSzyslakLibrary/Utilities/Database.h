#pragma once

#include "../Interfaces.h"
#include "../VLString.h"
#include "../VLVariable.h"
#include "VLFile.h"
#include "..\sqlite3.h"


class Row : public IUnknown
{
public:
	Row();
	~Row();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	void Set(wstring strName, wstring strVal, VLVariable::VAR_TYPE type);
	wstring Get(wstring strName);
	void Save(VLFile& fle);
	void Load(VLFile& fle, VLStringCollection& colNames);
	void WriteToSQL(wstring tbl, VLStringCollection& columns, sqlite3* db);

private:
	int m_cRef;
	VariableCollection* m_pValues;
};

class Table : public ITABLE
{
public:
	Table();
	~Table();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall NewRow();
	HRESULT __stdcall Set(const wchar_t* colName, const wchar_t* strVal, UINT nType);
	HRESULT __stdcall Get(const wchar_t* colName, IUnknown* iStrVal);
	HRESULT __stdcall GoToTopRow();
	HRESULT __stdcall RowCount(UINT* nRows);

	
	
	void Save(VLFile& fle);
	void Load(VLFile& fle);
	wstring Get(wstring colName);
	
	void NextRow();
	wstring Export();
	void Import(wstring xml);
	wstring GetColumn(int ndx);
	void WriteToSQL(wstring& errMsg, sqlite3* db);
	void ReadFromSQL(sqlite3* db);
	void Clear();

	inline wstring Name() { return m_name; }
	inline void Name(const wchar_t* szName) { m_name = szName; }

	

private:
	int m_cRef;
	wstring m_name;
	Row* m_pCurrentRow;
	VLStringCollection m_columns;
	InterfaceCollection m_rows;
	int m_nRowNdx = 0;

	void Export(VLFile& fle);
};

class Database : public IDATABASE
{
public:
	Database();
	~Database();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall GetTable(const wchar_t* szNme, IUnknown** iTbl);
	HRESULT __stdcall Load(const wchar_t* szFileNme);
	HRESULT __stdcall Save(const wchar_t* szFileNme);
	HRESULT __stdcall ReadFromSQL();
	
	wstring Export();
	void Import(wstring xml);
	void WriteToSQL();
	
	
	

private:
	int m_cRef;
	InterfaceCollection m_iTables;

};

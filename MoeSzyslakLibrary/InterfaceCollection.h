#pragma once

#include "Interfaces.h"
#include <map>

class InterfaceCollection : public InterfaceCollectionInf::IINTERFACECOLLECTION
{
public:
	InterfaceCollection();
	~InterfaceCollection();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Add(IUnknown* iObj, const wchar_t* szTag, UINT nClassID);
	HRESULT __stdcall Get(int ndx, IUnknown** iunk);
	HRESULT __stdcall Count(int* nCnt);
	HRESULT __stdcall Remove(int ndx);
	HRESULT __stdcall GetByTag(const wchar_t* szTag, IUnknown** iunk);
	HRESULT __stdcall ForEach(IUnknown** pObj);
	HRESULT __stdcall Clear();
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);
	HRESULT __stdcall UnitTest(IUnknown* iTst);
	HRESULT __stdcall Dispose();

	void GetLocalString(IUnknown** iStr);
	void RemoveByTag(const wchar_t* szTag);

protected:
	int m_cRef;
	int m_nObjects;
	int m_maxObjects;
	IUnknown** m_array;
	std::map<UINT, UINT> m_classIDs;
	int m_nCurrent;
	IUnknown* m_iRetString;
	StringInf m_iLocalStr;	
	std::map<std::wstring, int> m_mapToIndex;
	std::map<int, std::wstring> m_mapToTag;

	IUnknown* Get(std::wstring tag);
};


// This is an improved interface collection that adheres more closely to COM rules.
class ComCollection
{
public:
	ComCollection();
	~ComCollection();
	void Add(IUnknown* iObj, const wchar_t* szTag, UINT nClassID);
	void Clear();
	int Count();
	bool ForEach(IUnknown** ppObj);
	IUnknown* Get(int ndx);

private:
	std::vector<IUnknown*> m_array;
	std::map<UINT, UINT> m_classIDs;
	int m_nCurrent;
	std::map<std::wstring, int> m_mapToIndex;
	std::map<int, std::wstring> m_mapToTag;

	void Reset();
};

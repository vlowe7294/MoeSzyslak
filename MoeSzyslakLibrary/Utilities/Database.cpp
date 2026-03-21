#include "pch.h"
#include "Database.h"


Row::Row()
{
	m_cRef = 1;
	m_pValues = new VariableCollection();
}

Row::~Row()
{
	m_pValues->Release();
}

HRESULT __stdcall Row::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Row::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Row::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

void Row::Set(wstring strName, wstring strVal, VLVariable::VAR_TYPE type)
{
	m_pValues->Set(strName, strVal, type);
}

wstring Row::Get(wstring strName)
{
	return m_pValues->Get(strName);
}

void Row::Save(VLFile& fle)
{
	VLVariable* pVar = NULL;
	int nCol= 0;
	wstring c;

	m_pValues->Count(&nCol);
	fle.Write(nCol);

	while (m_pValues->ForEach(&pVar))
	{
		pVar->SaveToFile(fle);
	}
}

void Row::Load(VLFile& fle, VLStringCollection& colNames)
{
	int nCol = 0;
	fle.Read(nCol);
	m_pValues->Clear();
	StringInf iStr;
	UINT nType = 0;
	VLVariable* pVar = new VLVariable();

	for (int i = 0; i < nCol; i++)
	{		
		pVar->LoadFromFile(fle);
		pVar->GetType(&nType);
		m_pValues->Set(colNames.Get(i), pVar->GetString(), (VLVariable::VAR_TYPE)nType);
	}


	pVar->Release();

}




Table::Table()
{
	m_cRef = 1;
	m_pCurrentRow = new Row();
	m_rows.Add(m_pCurrentRow, L"", 0);
	m_nRowNdx = 0;
}

Table::~Table()
{
	m_pCurrentRow->Release();
}

HRESULT __stdcall Table::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == TABLE_IID)
	{
		*ppvObj = static_cast<ITABLE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Table::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Table::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Table::NewRow()
{
	m_pCurrentRow->Release();
	m_pCurrentRow = new Row();
	m_rows.Add(m_pCurrentRow, L"", 0);
	m_nRowNdx++;
	return S_OK;
}

HRESULT __stdcall Table::Set(const wchar_t* colName, const wchar_t* strVal, UINT nType)
{
	if (nType >= VLVariable::VAR_TYPE::TYPE_MAX)
		return E_FAIL;

	if (!m_columns.Find(colName))
		m_columns.Add(colName);

	m_pCurrentRow->Set(colName, strVal, (VLVariable::VAR_TYPE)nType);
	return S_OK;
}

wstring Table::Get(wstring colName)
{
	return m_pCurrentRow->Get(colName);

}

void Table::Export(VLFile& fle)
{
	wstring c;
	wstring lne;
	Row* pRow = NULL;

	while (m_columns.ForEach(c))
	{
		if (!lne.empty())
			lne += L",";

		lne += c;
	}

	fle.Write(lne);
	fle.Write(L"\n");
	

	while (m_rows.ForEach((IUnknown**)&pRow) == S_OK)
	{
		lne.clear();

		while (m_columns.ForEach(c))
		{
			if (!lne.empty())
				lne += L",";

			lne += pRow->Get(c);
		}

		fle.Write(lne);
		fle.Write(L"\n");
	}
}

void Table::Save(VLFile& fle)
{
	wstring c;
	Row* pRow = NULL;
	int nRows = 0;

	fle.Write(m_name);
	fle.Write(m_columns.GetCount());

	while (m_columns.ForEach(c))
	{
		fle.Write(c);
	}

	m_rows.Count(&nRows);
	fle.Write(nRows);

	while (m_rows.ForEach((IUnknown**)&pRow) == S_OK)
	{
		pRow->Save(fle);
	}
}

void Table::Load(VLFile& fle)
{
	wstring c;
	int nRows = 0, nCol = 0, i;
	VLString str;

	fle.Read(str);
	m_name = str.Get();
	m_columns.Clear();
	m_rows.Clear();

	m_pCurrentRow->Release();
	m_pCurrentRow = new Row();
	m_rows.Add(m_pCurrentRow, L"", 0);

	fle.Read(nCol);

	for (i = 0; i < nCol; i++)
	{
		fle.Read(str);
		m_columns.Add(str.Get().c_str());
	}

	fle.Read(nRows);

	for (i = 0; i < nRows; i++)
	{
		if (i > 0)
		{
			m_pCurrentRow->Release();
			m_pCurrentRow = new Row();
			m_rows.Add(m_pCurrentRow, L"", 0);
		}

		m_pCurrentRow->Load(fle, m_columns);
		m_rows.Add(m_pCurrentRow, L"", 0);

	}
}

void Table::GoToTopRow()
{
	m_pCurrentRow->Release();
	m_rows.Get(0, (IUnknown**)&m_pCurrentRow);
	m_pCurrentRow->AddRef();
	m_nRowNdx = 0;
}

void Table::NextRow()
{
	int nRows = 0;
	m_rows.Count(&nRows);

	m_nRowNdx++;	

	if (m_nRowNdx >= nRows)
		m_nRowNdx = 0;

	m_pCurrentRow->Release();
	m_rows.Get(m_nRowNdx, (IUnknown**)&m_pCurrentRow);
	m_pCurrentRow->AddRef();

}

wstring Table::Export()
{
	wstring ret, c;
	Row* pRow = NULL;
	int nRows = 0;

	ret = L"<table>\n   <name>" + m_name + L"</name>\n";

	while (m_rows.ForEach((IUnknown**)&pRow) == S_OK)
	{
		ret += L"   <row>\n   ";
		
		while (m_columns.ForEach(c))
		{
			ret += L"      <" + c + L">";
			ret += pRow->Get(c);
			ret += L"</" + c + L">\n";			
		}
		
		ret += L"   </row>\n";		
	}

	ret += L"</table>";
	return ret;
}

void Table::Import(wstring strXml)
{
	int s = strXml.find(L"<name", 0), e;
	wstring col, val;

	if (s >= 0)
	{
		s = strXml.find(L">", s);
		e = strXml.find(L"</name", s);
		m_name = strXml.substr(s + 1, e - s - 1);
	}

	s = strXml.find(L"<row", 0);

	while (s > 0)
	{
		e = strXml.find(L"</row", s);
		s = strXml.find(L"<", s + 1);

		while (s < e)
		{
			int colE = strXml.find(L">", s);
			col = strXml.substr(s + 1, colE - s - 1);
			
			s = colE;
			colE = strXml.find(L"<", s);
			val = strXml.substr(s + 1, colE - s - 1);
			s = strXml.find(L"<", colE + 1);

			Set(col.c_str(), val.c_str(), VLVariable::VAR_TYPE::TYPE_STRING);
		}

		s = strXml.find(L"<row", e);
	}
}

wstring Table::GetColumn(int ndx)
{
	return m_columns.Get(ndx);
}






Database::Database()
{
	m_cRef = 1;
	IUnknown* iUnk = NULL;
}

Database::~Database()
{
	
}

HRESULT __stdcall Database::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == DATABASE_IID)
	{
		*ppvObj = static_cast<IDATABASE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall Database::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Database::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

HRESULT __stdcall Database::GetTable(const wchar_t* szNme, IUnknown** iTbl)
{
	Table* pTbl = NULL;

	m_iTables.GetByTag(szNme, (IUnknown**)&pTbl);

	if (pTbl == NULL)
	{
		pTbl = new Table();
		pTbl->Name(szNme);
		m_iTables.Add(pTbl, szNme, 0);
		pTbl->Release();
	}

	pTbl->QueryInterface(IID_IUnknown, (void**)iTbl);
	pTbl->Release();
	return S_OK;
}

HRESULT __stdcall Database::Load(const wchar_t* szFileNme)
{
	EncryptedFile fle;
	Table* pTbl = NULL;
	int nTbls = 0;

	fle.Open(szFileNme);
	m_iTables.Clear();

	if (!fle.IsOpen())
		return E_FAIL;

	fle.Read(nTbls);

	for (int i = 0; i < nTbls; i++)
	{
		Table* pTbl = new Table();
		pTbl->Load(fle);
		m_iTables.Add(pTbl, pTbl->Name().c_str(), 0);
		pTbl->Release();
	}

	return S_OK;
}

HRESULT __stdcall Database::Save(const wchar_t* szFileNme)
{
	EncryptedFile fle;
	Table* pTbl = NULL;
	int nTbls = 0;

	fle.Create(szFileNme);
	m_iTables.Count(&nTbls);
	fle.Write(nTbls);

	while (m_iTables.ForEach((IUnknown**)&pTbl) == S_OK)
	{
		pTbl->Save(fle);
	}

	return S_OK;

}

wstring Database::Export()
{
	Table* pTbl = NULL;
	int nTbls = 0;
	wstring strXml = L"<tables>\n";
	VLString txml;


	while (m_iTables.ForEach((IUnknown**)&pTbl) == S_OK)
	{
		strXml += L"   ";
		txml.Set(pTbl->Export().c_str());
		txml.Replace(L"\n", L"\n   ");

		strXml += txml.Get();
		strXml += L"\n";
	}

	strXml += L"</tables>";
	return strXml;
}

void Database::Import(wstring strXml)
{
	wstring sec;

	int s = strXml.find(L"<tables"), e;

	if (s < 0)
		return;

	s = strXml.find(L">");
	e = strXml.find(L"</tables");

	strXml = strXml.substr(s, e - s);

	s = strXml.find(L"<table", 0);

	while (s >= 0)
	{
		e = strXml.find(L"</table", s);
		Table* pTbl = new Table();
		pTbl->Import(strXml.substr(s, e - s));

		m_iTables.Add(pTbl, pTbl->Name().c_str(), 0);
		pTbl->Release();
		s = strXml.find(L"<table", e);

	}
	

}


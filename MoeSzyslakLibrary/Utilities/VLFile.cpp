#include "pch.h"
#include <fstream>
#include "VLFile.h"

VLFile::VLFile()
{
	m_fileStats.st_size = 0;
	m_pos = 0;
	m_pOutFile = NULL;
	m_pInFile = NULL;
	m_cRef = 1;
}

HRESULT __stdcall VLFile::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall VLFile::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall VLFile::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

VLFile::~VLFile()
{
	Close();
}

void VLFile::Create(wstring filePath)
{
	Close();

	m_pOutFile = new std::ofstream();
	m_pos = 0;
	m_fileStats.st_size = 0;

	m_pOutFile->open(filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	if (!m_pOutFile->is_open())
	{
		Close();
		return;
	}
}

void VLFile::Close()
{
	m_fileStats.st_size = 0;
	m_pos = 0;

	if (m_pOutFile != NULL)
	{
		m_pOutFile->close();
		delete m_pOutFile;
		m_pOutFile = NULL;
	}

	if (m_pInFile != NULL)
	{
		m_pInFile->close();
		delete m_pInFile;
		m_pInFile = NULL;
	}
}

void VLFile::Write(wstring stdStr)
{
	int len = int(stdStr.length()) * 2;
	
	if (m_pOutFile == NULL)
		return;

	m_pOutFile->write((const char*)(stdStr.c_str()), len);
	m_pos += len;
	m_fileStats.st_size += len;
}

void VLFile::WriteLine(wstring str)
{
	int len = int(str.length()) * 2;
	wchar_t endChar = L'\n';

	if (m_pOutFile == NULL)
		return;

	m_pOutFile->write((const char*)str.c_str(), len);
	m_pos += len;
	m_fileStats.st_size += len;

	m_pOutFile->write((const char*)&endChar, 2);
	m_pos += 2;
}

wstring VLFile::ReadLine()
{
	wstring str;

	if (m_pInFile == NULL)
		return str;

	wchar_t endChar = L'\n', c = L'\0';

	while (c != endChar && IsOpen())
	{
		if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
			Close();
		else
		{
			m_pInFile->read((char*)&c, 2);
			if (c != endChar)
				str += c;

			m_pos += 2;
		}

	}

	return str;
}

void VLFile::Write(int nVal)
{
	if (m_pOutFile == NULL)
		return;

	m_pOutFile->write((const char*)&nVal, 4);
	m_pos += 4;
	m_fileStats.st_size += 4;
}

void VLFile::Write(double fVal)
{
	if (m_pOutFile == NULL)
		return;

	m_pOutFile->write((const char*)&fVal, 8);
	m_pos += 8;
	m_fileStats.st_size += 8;
}

void VLFile::Read(VLString& str)
{
	int len = int(str.GetLength()) * 2;
	wchar_t endChar = L'\0', c = L'\0';

	wstring s;

	str.Set(s.c_str());

	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)&c, 2);

	while (c != endChar && m_pos <= (unsigned long long)m_fileStats.st_size)
	{
		s += c;
		m_pos += 2;
		m_pInFile->read((char*)&c, 2);
	}

	m_pos += 2;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

	str.Set(s.c_str());
}

void VLFile::Read(string& str)
{
	char endChar = '\0', c;

	str.clear();

	if (m_pInFile == NULL)
		return;

	m_pos += 1;
	m_pInFile->read((char*)&c, 1);

	while (m_pos <= (unsigned long long)m_fileStats.st_size && c != endChar)
	{
		str += c;
		m_pos += 1;
		m_pInFile->read((char*)&c, 1);
	}

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

}

void VLFile::Read(int& nVal)
{
	nVal = 0;

	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)&nVal, 4);
	m_pos += 4;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

}

void VLFile::Read(UINT& nVal)
{
	nVal = 0;

	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)&nVal, 4);
	m_pos += 4;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

}

void VLFile::Read(double& fVal)
{
	fVal = 0.0;

	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)&fVal, 8);
	m_pos += 8;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

}

void VLFile::Open(wstring filePath)
{
	Close();

	if (_wstat64(filePath.c_str(), &m_fileStats) != 0)
		return;

	m_pInFile = new std::ifstream();
	m_pInFile->open((LPCWSTR)filePath.c_str(), std::ios::in | std::ios::binary);

	if (!m_pInFile->is_open())
	{
		Close();
		return;
	}

	m_fullPath.Set(filePath.c_str());
	m_pos = 0;
}

void VLFile::Write(unsigned char* pData, UINT nLen)
{
	if (m_pOutFile == NULL)
		return;

	m_pOutFile->write((const char*)pData, nLen);
	m_pos += nLen;
	m_fileStats.st_size += nLen;
}

void VLFile::Read(unsigned char* pData, UINT nLen)
{
	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)pData, nLen);
	m_pos += nLen;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();
}

bool VLFile::DirectoryExists(wstring& dirPath)
{
	if (dirPath.size() == 0)
		dirPath = L".\\";

	if (dirPath[dirPath.size() - 1] != L'\\')
		dirPath += L"\\";


	DWORD dwAttrib = GetFileAttributes(dirPath.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool VLFile::FileExists(wstring filePath)
{
	struct __stat64 fileStats;

	if (filePath.size() == 0)
		return false;

	if (_wstat64(filePath.c_str(), &fileStats) != 0)
		return false;

	return true;

}




unsigned char EncryptedFile::m_key[100] =
{
	201,246,200,168,18,188,190,122,228,51,98,59,129,3,55,170,
	12,64,246,144,14,163,174,125,26,183,253,43,41,98,234,110,
	220,85,194,153,187,181,23,172,30,243,210,231,85,33,119,252,
	254,206,238,220,152,201,226,93,39,220,151,195,173,171,197,98,
	139,184,244,98,206,134,211,201,149,20,196,132,79,16,151,67,
	86,128,90,26,158,187,178,93,160,71,51,82,7,99,70,121,
	28,166,225,172
};

EncryptedFile::EncryptedFile()
{
	m_nKeyNdx = 0;
	m_pBytes = new unsigned char[100];
	m_maxBytes = 100;
}

EncryptedFile::~EncryptedFile()
{

}

void EncryptedFile::Create(wstring filePath)
{
	unsigned char k = '2';
	VLFile::Create(filePath);
	VLFile::Write(&k, 1);
	m_nKeyNdx = 0;
}

void EncryptedFile::Open(wstring filePath)
{
	unsigned char k = 0;
	VLFile::Open(filePath);

	if (!IsOpen())
		return;

	VLFile::Read(&k, 1);
	m_nKeyNdx = 0;
}

void EncryptedFile::Write(int nVal)
{
	if (m_pOutFile == NULL)
		return;

	memcpy(m_pBytes, &nVal, 4);
	EncryptBytes(4);

	m_pOutFile->write((const char*)m_pBytes, 4);
	m_pos += 4;
	m_fileStats.st_size += 4;
}

void EncryptedFile::Write(wstring stdStr)
{
	VLString str;

	str.Set(stdStr.c_str());
	int len = int(str.GetLength()) * 2;
	wchar_t endChar = L'\0';

	if (m_pOutFile == NULL)
		return;

	while (m_maxBytes < len + 2)
	{
		delete m_pBytes;
		m_maxBytes = m_maxBytes * 2;
		m_pBytes = new unsigned char[m_maxBytes * 2];
	}

	memcpy(m_pBytes, stdStr.c_str(), len);
	memcpy(&m_pBytes[len], &endChar, 2);
	EncryptBytes(len + 2);

	m_pOutFile->write((const char*)m_pBytes, len + 2);
	m_pos += len + 2;
	m_fileStats.st_size += len + 2;
}

void EncryptedFile::Read(int& nVal)
{
	if (m_pInFile == NULL)
		return;

	m_pInFile->read((char*)m_pBytes, 4);
	m_pos += 4;

	if (m_pos >= (unsigned long long)m_fileStats.st_size || m_pInFile->eof())
		Close();

	DecryptBytes(4);
	memcpy(&nVal, m_pBytes, 4);
}

void EncryptedFile::Read(VLString& str)
{
	wchar_t endChar = L'\0', c = L'\0';
	wstring s;

	if (m_pInFile == NULL)
		return;

	Read(c);

	while (c != L'\0')
	{
		s += c;
		Read(c);
	}

	str.Set(s.c_str());
}

void EncryptedFile::Read(wchar_t& c)
{
	m_pInFile->read((char*)m_pBytes, 2);
	m_pos += 2;
	DecryptBytes(2);
	memcpy(&c, m_pBytes, 2);
}

void EncryptedFile::EncryptBytes(int nLen)
{
	int nTmp = 0;

	for (int i = 0; i < nLen; i++)
	{
		if (m_nKeyNdx == 100)
			m_nKeyNdx = 0;

		nTmp = m_pBytes[i] + m_key[m_nKeyNdx];

		if (nTmp > 250)
			nTmp -= 250;

		m_pBytes[i] = nTmp;
		m_nKeyNdx++;
	}
}

void EncryptedFile::DecryptBytes(int nLen)
{
	int nTmp;

	for (int i = 0; i < nLen; i++)
	{
		if (m_nKeyNdx == 100)
			m_nKeyNdx = 0;

		nTmp = m_pBytes[i] - m_key[m_nKeyNdx];

		if (nTmp < 0)
			nTmp = 250 + nTmp;

		m_pBytes[i] = nTmp;
		m_nKeyNdx++;
	}

}

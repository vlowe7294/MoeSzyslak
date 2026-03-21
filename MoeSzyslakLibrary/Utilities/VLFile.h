#pragma once

#include "..\Interfaces.h"
#include "..\VLString.h"

class VLFile : public IUnknown
{
public:
	VLFile();
	~VLFile();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();


	virtual void Create(wstring filePath);
	virtual void Write(wstring str);
	virtual void Write(int nVal);
	void Write(double fVal);
	virtual void Read(VLString& str);
	virtual void Read(string& str);
	virtual void Read(int& nVal);
	void Read(UINT& nVal);
	void Read(double& fVal);
	virtual void Open(wstring filePath);
	void WriteLine(wstring str);
	wstring ReadLine();

	void Write(unsigned char* pData, UINT nLen);
	void Read(unsigned char* pData, UINT nLen);
	static bool DirectoryExists(wstring& dirPath);
	static bool FileExists(wstring filePath);

	inline bool IsOpen() { return (m_pInFile != NULL || m_pOutFile != NULL); };

protected:
	int m_cRef;
	struct __stat64 m_fileStats;
	unsigned long long m_pos;
	std::ofstream* m_pOutFile;
	std::ifstream* m_pInFile;

	void Close();
	VLString m_fullPath;
};

class EncryptedFile : public VLFile
{
public:
	EncryptedFile();
	~EncryptedFile();
	void Write(int nVal);
	void Write(wstring str);
	void Create(wstring filePath);
	void Open(wstring filePath);
	void Read(int& nVal);
	void Read(VLString& str);

private:
	static unsigned char m_key[100];
	const int m_keySize = 100;
	int m_nKeyNdx;
	unsigned char* m_pBytes;
	int m_maxBytes;

	void EncryptBytes(int nLen);
	void DecryptBytes(int nLen);
	void Read(wchar_t& c);
};

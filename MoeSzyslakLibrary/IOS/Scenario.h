#pragma once

#include <WS2tcpip.h>
#include "..\VLVariable.h"
#include "PDU.h"


enum SimState
{
	STATE_NO_STATE,
	STATE_READY,
	STATE_FROZEN,
	STATE_STARTED,
	STATE_NUM_STATES
};

class SocketHandler
{
public:
	SocketHandler();
	~SocketHandler();
	bool openSocket();
private:
	bool m_disOpen;
	string dis_ip;
	int dis_port;
	WSADATA wsa_data;
};

class DISNetwork
{
public:
	DISNetwork();
	~DISNetwork();
	void AddPDU(PDU& pdu);
	bool GetPDU(UINT id, PDU& pdu);
	void Listen();

private:
	InterfaceCollection m_pdus;
	SocketHandler m_socketHandler;
	bool m_bRecord;

};


class RemoteDeviceModelF16 : public IUnknown
{
public:
	RemoteDeviceModelF16();
	~RemoteDeviceModelF16();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void Update();

	inline DISNetwork& GetDISNetwork() { return m_disNetwork; };
	inline wstring DeviceName() { return m_deviceName; };
	inline SimState State() { return m_state; };

private:
	int m_cRef;
	DISNetwork m_disNetwork;
	wstring m_deviceName;
	SimState m_state;
	static const int OwnershipStatusDatumID = 15800;
};


class Scenario : public IUnknown
{
public:
	Scenario();
	~Scenario();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void Command(wstring szCmd);
	

	inline void SelectedTrainer(RemoteDeviceModelF16* pTrainer) 
	{
		if (m_pSelectedTrainer != NULL)
			m_pSelectedTrainer->Release();

		m_pSelectedTrainer = pTrainer; 

		if (m_pSelectedTrainer != NULL)
			m_pSelectedTrainer->AddRef();
	};

	inline VariableCollection& Properties() { return m_properties; }
	inline void TES(wstring strTES) { m_tes = strTES; }

private:
	int m_cRef;
	VariableCollection m_properties;

	VLVariable* m_pRecord;
	VLVariable* m_pICFilePath;

	wstring m_selectedTrainer;
	RemoteDeviceModelF16* m_pSelectedTrainer;
	wstring m_tes;
};

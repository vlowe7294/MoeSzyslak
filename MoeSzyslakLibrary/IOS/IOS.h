#include "..\Interfaces.h"
#include "..\VLVariable.h"
#include "CommGeneralPage.h"
#include "Scenario.h"

#pragma once

class IosAssignRolesPage
{
public:
	IosAssignRolesPage();
	~IosAssignRolesPage();

	inline void SteamGauges(wstring strSteamGauges) { m_SteamGaugesCombo = strSteamGauges; }
	inline void FCRCombo(wstring strFCRCombo) { m_FCRCombo = strFCRCombo; }

private:
	wstring m_SteamGaugesCombo;
	wstring m_FCRCombo;
};




class IosChooseSfesPage
{
public:
	IosChooseSfesPage();
	~IosChooseSfesPage();
	void Update();
	
	inline void ScenarioPath(wstring strPath) { m_scenarioPath = strPath; }

private:
	wstring m_scenarioPath;
	wstring m_IcDescription;
};






class IosEstablishWizard
{
public:
	IosEstablishWizard();
	~IosEstablishWizard();
	int DoModal(wstring scenarioPath);

	inline void AVSAF(wstring strAvsaf) { m_avsaf = strAvsaf; }

private:
	IosChooseSfesPage m_IosChooseSfesPage;	
	
	IosAssignRolesPage m_IosAssignRolesPage;	
	wstring m_avsaf;

};




class NetworkDevices
{
public:
	NetworkDevices();
	~NetworkDevices();
	static NetworkDevices& GetNetworkDevice();
	void Add(RemoteDeviceModelF16& f16);
	bool ForEach(RemoteDeviceModelF16** pF16);
	

private:
	static NetworkDevices* m_pTheNetworkDevices;
	InterfaceCollection m_devices;

};




class IOS : public IOSInf::IIOS
{
public:
	IOS();
	~IOS();

	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall Properties(IUnknown** iProp);
	HRESULT __stdcall UnitTest();
	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall Update();

	HRESULT __stdcall EstablishExercise();
	

private:
	int m_cRef;
	VariableCollection m_properties;
	CommsInf m_iCommGeneralPage;	
	LocalDeviceModelIos m_iosDevice;
	RemoteDeviceModelF16* m_pF16Device;
	Scenario* m_pScenario;
	
};

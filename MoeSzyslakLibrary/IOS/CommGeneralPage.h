#pragma once

#include "..\VLVariable.h"
#include "Scenario.h"

enum AudioMode
{
	AUDIO_MODE_BACKUP,
	AUDIO_MODE_NORMAL
};

enum RadioModeType
{
	OFF = 0,
	FM_,
	VHF,
	UHF,
	SINCGARS,
	HAVEQUICK,
	SATCOM,
	SATURN,
	MAX
};

enum ModulationType
{
	NONE = 0,
	AM,
	FM
};

enum ICDRadioMode 
{
	//! Off
	AUDIO_RADIO_MODE_OFF = 0,

	//! FM
	AUDIO_RADIO_MODE_FM = 1,

	//! VHF: FM
	AUDIO_RADIO_MODE_VHF_FM = 2,

	//! VHF: AM
	AUDIO_RADIO_MODE_VHF_AM = 3,

	//! UHF: FM
	AUDIO_RADIO_MODE_UHF_FM = 4,

	//! UHF: AM
	AUDIO_RADIO_MODE_UHF_AM = 5,

	//! SINCGARS
	AUDIO_RADIO_MODE_SINCGARS = 6,

	//! Have Quick
	AUDIO_RADIO_MODE_HAVEQUICK = 7,

	//! Intercom
	AUDIO_RADIO_MODE_INTERCOM = 8,

	//! IDM
	AUDIO_RADIO_MODE_IDM = 9,

	//! IDT
	AUDIO_RADIO_MODE_IDT = 10,

	//! SATCOM 
	AUDIO_RADIO_MODE_SATCOM = 11,

	AUDIO_RADIO_MODE_LAST
};

enum DatumIDAudio
{
	DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_1_IOS = 701005,
	DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_2_IOS = 701007,
	DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_3_IOS = 701009,
	DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_1_IOS = 701011,
	DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_2_IOS = 701013,
	DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_3_IOS = 701015,
	DATUM_ID_FIXED_AUDIO_VOLUME_ENVIRONMENTAL = 701029,
	DATUM_ID_FIXED_AUDIO_VOLUME_IOS_CUES_TONES = 701030,
	DATUM_ID_FIXED_AUDIO_VOLUME_COMM_1 = 701036,
	DATUM_ID_FIXED_AUDIO_VOLUME_COMM_2 = 701037,
	DATUM_ID_FIXED_AUDIO_VOLUME_COMM_3 = 701038,
	DATUM_ID_FIXED_AUDIO_VOLUME_INTERCOM = 701039,
	DATUM_ID_FIXED_AUDIO_AURAL_CUES_COCKPIT_SELECT = 701096,
	DATUM_ID_FIXED_AUDIO_OPERATIONAL_MODE_IOS = 701098
};


class Radio : public IUnknown
{
public:
	Radio(int nIndex);
	~Radio();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	void Commit(DISNetwork& dis);
	void Update(DISNetwork& dis);
	void SetPreset(int ndx, double fFreq);
	inline double GetPresetFrequncy(int ndx)
	{
		if (ndx < 0 || ndx >= 20)
			return 0.0;

		return m_presetFrequencyValues[ndx];
	}

	void SetFrequencyFromPreset(int ndx);
	void Properties(IUnknown** iPrp);

	inline double Frequency() { return m_frequency; }
	void Frequency(double fFrequency);
		
	inline RadioModeType Mode() { return m_mode; }
	inline void Mode (RadioModeType nMode) { m_mode = nMode; }

private:
	int m_cRef;
	const double COMM_1_FREQ_MAX = 399.975; //AM
	const double COMM_1_FREQ_MIN = 225.000; //AM
	const double COMM_2_ILS_MIN = 108.000; //AM
	
	int m_ndx;

	VLVariable* m_pVolume;

	double m_frequency;	
	double m_presetFrequencyValues[20];
	VariableCollection m_properties;
	RadioModeType m_mode;
	ModulationType m_eModulation; 

	ICDRadioMode ConvertToICDRadioMode();
	void ConvertFromIOSRadioMode(int radioMode);
};

enum SimModeTransition
{
	ILLEGAL_MODE_TRANSITION = -1,
	TO_STANDBY,
	TO_DIAGNOSTIC,
	TO_PLAN,
	TO_TRAIN,
	TO_REPLAY,
	TO_AAR,
	TO_OBSERVATION,
	TO_BRIEF,
	TO_RECOVERY,
	TO_AUTO_DEMO,
	MODE_NUM_TOTAL_TRANSITIONS
};

class LocalDeviceModelIos
{
public:
	LocalDeviceModelIos();
	~LocalDeviceModelIos();
	
	void SetAudioMode(AudioMode mde);
	void AuralCues(int nAuralCues);
	void AuralCuesVolume(int nAuralCuesVol);
	void IntercomVolume(int nIntercomVolume);
	inline void SetScenarioSetup(Scenario* pScenario) { m_pScenario = pScenario; };
	inline void Mode(SimModeTransition mde) { m_mode = mde; };

private:
	DISNetwork m_disNetwork;
	SimModeTransition m_mode;
	AudioMode m_AudioMode;
	int m_nAuralCues;
	int m_nAuralVolume;
	int m_nIntercomVolume;
	Scenario* m_pScenario;
};

class CommGeneralPage : public ICOMMGENERALPAGE
{
public:
	CommGeneralPage();
	~CommGeneralPage();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);	
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall UnitTest();
	HRESULT __stdcall Commit();

	
	void Update();
	void Command(const wchar_t* szCmd);
	wstring GetReturnString();

	inline void SetMatchingOwnship(RemoteDeviceModelF16* pDev) { m_pMatchingOwnship = pDev; };
	inline void SetLocalDevice(LocalDeviceModelIos* pDev) { m_pIOSDevice = pDev; };

private:
	int m_cRef;
	VariableCollection* m_pProperties;
	VLVariable* m_pBackupAudio;
	VLVariable* m_pSelectAuralCues;
	VLVariable* m_pAuralCueEnabled;
	VLVariable* m_pAuraCuesVolume;
	VLVariable* m_pIntercomEnabled;
	VLVariable* m_pIntercomVolume;

	InterfaceCollection m_radios;

	RemoteDeviceModelF16* m_pMatchingOwnship;
	LocalDeviceModelIos* m_pIOSDevice;
	StringInf m_iRetString;
};

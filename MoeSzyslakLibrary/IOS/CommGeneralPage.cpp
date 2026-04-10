#include "../pch.h"
#include "CommGeneralPage.h"

#pragma comment(lib,"WS2_32")

SocketHandler::SocketHandler()
{
	m_disOpen = false;
	dis_ip = "172.16.53.255";
	dis_port = 6993;
}

SocketHandler::~SocketHandler()
{

}

bool SocketHandler::openSocket()
{
	int ws_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (ws_result != 0)
		return false;

	m_disOpen = true;
	return true;

}






DISNetwork::DISNetwork() 
{
	m_bRecord = true;
}

DISNetwork::~DISNetwork()
{

}

void DISNetwork::AddPDU(PDU& pdu)
{
	wstring sID = std::to_wstring(pdu.ID());
	PDU* pPdu = NULL;

	m_pdus.GetByTag(sID.c_str(), (IUnknown**)&pPdu);

	if (pPdu == NULL)
	{
		pPdu = new PDU(pdu.ID());
		m_pdus.Add(pPdu, sID.c_str(), 0);
		pPdu->Release();
	}

	pPdu->Compare(pdu);

	pPdu->Copy(pdu);
}

bool DISNetwork::GetPDU(UINT id, PDU& pdu)
{
	wstring sID = std::to_wstring(pdu.ID());
	PDU* pPdu = NULL;

	m_pdus.GetByTag(sID.c_str(), (IUnknown**)&pPdu);

	if (pPdu == NULL)
		return false;

	pdu.Copy(*pPdu);
	return true;
}

void DISNetwork::Listen()
{
	m_socketHandler.openSocket();
}




Radio::Radio(int nIndex)
{
	m_cRef = 1;
	
	VLVariable* v = m_properties.NewVariable(L"Enabled");
	v->SetAsBool(FALSE);

	if (nIndex > 0 && nIndex < 4)
		m_ndx = nIndex;
	else
		m_ndx = 0;

	v = m_properties.NewVariable(L"Volume");
	v->SetAsInt(0, 0);
	v->SetLimits(0, 100);

	memset(m_presetFrequencyValues, 0, sizeof(double) * 20);
	SetPreset(1, 230.0);
	m_mode = RadioModeType::OFF;
	m_eModulation = ModulationType::AM;
}

Radio::~Radio()
{

}

HRESULT __stdcall Radio::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall Radio::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall Radio::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
	}

	return m_cRef;
}

void Radio::Commit(DISNetwork& dis)
{
	DatumIDAudio dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_1;
	BOOL b = FALSE;
	double d = 0.0;
	
	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_2;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_3;

	PDU pdu(dt);
	VariableInf v;
	m_properties.GetByTag(L"Enabled", v);
	
	v->GetAsBool(&b);

	m_properties.GetByTag(L"Volume", v);	

	if (b == FALSE)
		v->SetAsInt(0, 0);

	v->GetAsFloat(0, &d);

	pdu.Value(d / 100.0);
	dis.AddPDU(pdu);

	dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_1_IOS;

	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_2_IOS;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_3_IOS;

	PDU RadioFrequency(dt);
	RadioFrequency.Value(m_frequency);
	dis.AddPDU(RadioFrequency);

	dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_1_IOS;

	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_2_IOS;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_3_IOS;

	PDU RadioMode(dt);
	RadioMode.Value(ConvertToICDRadioMode());
	dis.AddPDU(RadioMode);

}

void Radio::Update(DISNetwork& dis)
{
	DatumIDAudio dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_1;
	PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_VOLUME_COMM_1);
	int vol;

	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_2;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_VOLUME_COMM_3;

	VariableInf v;
	m_properties.GetByTag(L"Enabled", v);	

	if (dis.GetPDU(dt, pdu))
	{
		vol = pdu.GetFloatValue() * 100.0;	

		if (vol > 0)
			v->SetAsBool(TRUE);
		else
			v->SetAsBool(FALSE);

		m_properties.GetByTag(L"Volume", v);
		v.SetInt(vol, 0);
	}

	dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_1_IOS;

	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_2_IOS;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_FREQUENCY_COMM_3_IOS;

	if (dis.GetPDU(dt, pdu))
		m_frequency = pdu.GetFloatValue();

	dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_1_IOS;

	if (m_ndx == 1)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_2_IOS;
	else if (m_ndx == 2)
		dt = DATUM_ID_FIXED_AUDIO_RADIO_MODE_COMM_3_IOS;

	if (dis.GetPDU(dt, pdu))
		ConvertFromIOSRadioMode(pdu.GetIntValue());
}

void Radio::SetPreset(int ndx, double fFreq)
{
	if (ndx < 1 || ndx > 19)
		return;

	if (m_ndx == 0)
	{
		if (COMM_1_FREQ_MIN <= fFreq && fFreq <= COMM_1_FREQ_MAX)
		{
			m_presetFrequencyValues[ndx] = fFreq;
		}
	}
	else
	{
		if (COMM_2_ILS_MIN <= fFreq && fFreq <= COMM_1_FREQ_MAX)
		{
			m_presetFrequencyValues[ndx] = fFreq;
		}
	}
	
}

void Radio::SetFrequencyFromPreset(int ndx)
{
	if (ndx < 1 || ndx > 19 || m_presetFrequencyValues[ndx] == 0)
		return;

	Frequency(m_presetFrequencyValues[ndx]);
}

void Radio::Frequency(double fFreq)
{
	int n = fFreq * 1000;
	// frequency has to be divisible by 0.025;
	int mod = n % 25;

	if (mod > 0)
	{
		n = n - mod + 25;
		fFreq = (double)n / 1000.0;
	}


	if (m_ndx == 0)
	{
		if (COMM_1_FREQ_MIN <= fFreq && fFreq <= COMM_1_FREQ_MAX)
		{
			m_frequency = fFreq;
		}
	}
	else
	{
		if (COMM_2_ILS_MIN <= fFreq && fFreq <= COMM_1_FREQ_MAX)
		{
			m_frequency = fFreq;
		}
	}

}

void Radio::Properties(IUnknown** iPrp)
{
	m_properties.QueryInterface(IID_IUnknown, (void**)iPrp);
	m_properties.Release();
}

ICDRadioMode Radio::ConvertToICDRadioMode()
{
	ICDRadioMode eMode = ICDRadioMode::AUDIO_RADIO_MODE_OFF;

	switch (m_mode)
	{
	case RadioModeType::FM_:
		eMode = ICDRadioMode::AUDIO_RADIO_MODE_FM;
		break;
	case RadioModeType::VHF:
		if (m_eModulation == ModulationType::AM)
		{
			eMode = ICDRadioMode::AUDIO_RADIO_MODE_VHF_AM;
		}
		else
		{
			eMode = ICDRadioMode::AUDIO_RADIO_MODE_VHF_FM;
		}
		break;
	case RadioModeType::UHF:
		if (m_eModulation == ModulationType::AM)
		{
			eMode = ICDRadioMode::AUDIO_RADIO_MODE_UHF_AM;
		}
		else
		{
			eMode = ICDRadioMode::AUDIO_RADIO_MODE_UHF_FM;
		}
		break;
	case RadioModeType::HAVEQUICK:
		eMode = ICDRadioMode::AUDIO_RADIO_MODE_HAVEQUICK;
		break;
	case RadioModeType::SINCGARS:
		eMode = ICDRadioMode::AUDIO_RADIO_MODE_SINCGARS;
		break;
	case RadioModeType::SATCOM:
		eMode = ICDRadioMode::AUDIO_RADIO_MODE_SATCOM;
		break;
	case RadioModeType::SATURN:
		// Using sincgars for this selection until a new icdDIS is added for Saturn
		eMode = ICDRadioMode::AUDIO_RADIO_MODE_SINCGARS;
		break;
	}

	return eMode;
}

void Radio::ConvertFromIOSRadioMode(int radioMode)
{
	switch (radioMode)
	{
	case ICDRadioMode::AUDIO_RADIO_MODE_FM:
		m_mode = RadioModeType::FM_;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_HAVEQUICK:
		m_mode = RadioModeType::HAVEQUICK;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_OFF:
		m_mode = RadioModeType::OFF;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_SINCGARS:
		m_mode = RadioModeType::SINCGARS;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_UHF_AM:
	case ICDRadioMode::AUDIO_RADIO_MODE_UHF_FM:
		m_mode = RadioModeType::UHF;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_VHF_AM:
	case ICDRadioMode::AUDIO_RADIO_MODE_VHF_FM:
		m_mode = RadioModeType::VHF;
		break;
	case ICDRadioMode::AUDIO_RADIO_MODE_SATCOM:
		m_mode = RadioModeType::SATCOM;
		break;
	}
}








CommGeneralPage::CommGeneralPage()
{
	m_cRef = 1;
	m_pProperties = new VariableCollection();

	m_pBackupAudio = m_pProperties->NewVariable(L"Backup Audio Enabled");
	m_pBackupAudio->SetAsBool(FALSE);

	m_pSelectAuralCues = m_pProperties->NewVariable(L"Cockpit Selection");
	m_pSelectAuralCues->SetString(L"None");
	
	m_pAuralCueEnabled = m_pProperties->NewVariable(L"Aural Cues Enabled");
	m_pAuralCueEnabled->SetAsBool(FALSE);

	m_pAuraCuesVolume = m_pProperties->NewVariable(L"Aural Cues Volume");
	m_pAuraCuesVolume->SetLimits(0, 100);
	m_pAuraCuesVolume->SetAsInt(0, 0);

	m_pIntercomEnabled = m_pProperties->NewVariable(L"Intercom Enabled");
	m_pIntercomEnabled->SetAsBool(FALSE);

	m_pIntercomVolume = m_pProperties->NewVariable(L"Intercom Volume");
	m_pIntercomVolume->SetLimits(0, 100);
	m_pIntercomVolume->SetAsInt(0, 0);

	m_pMatchingOwnship = NULL;
	m_pIOSDevice = NULL;
	m_iRetString.Init();

	Radio* r = new Radio(0);
	VariableInfCollection iPrp;

	r->Properties(iPrp);
	iPrp.SetBool(L"Enabled", true);
	iPrp.Set(L"Volume", L"50");
	m_radios.Add(r, L"radio 1", 0);

	r = new Radio(1);
	r->Properties(iPrp);
	iPrp.SetBool(L"Enabled", true);

	iPrp.Set(L"Volume", L"50");
	m_radios.Add(r, L"radio 2", 0);

	r = new Radio(2);
	r->Properties(iPrp);
	iPrp.SetBool(L"Enabled", true);

	iPrp.Set(L"Volume", L"50");
	m_radios.Add(r, L"radio 3", 0);


}

CommGeneralPage::~CommGeneralPage()
{
	m_pProperties->Release();

}

HRESULT __stdcall CommGeneralPage::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
		return NOERROR;
	}
	else if (riid == COMMGENERAL_IID)
	{
		*ppvObj = static_cast<ICOMMGENERALPAGE*>(this);
		AddRef();
		return NOERROR;
	}
	else
		return E_NOINTERFACE;

}

ULONG __stdcall CommGeneralPage::AddRef()
{
	m_cRef++;
	return m_cRef;
}

ULONG __stdcall CommGeneralPage::Release()
{
	m_cRef--;

	if (0 == m_cRef)
	{
		delete this;
	}

	return m_cRef;
}

HRESULT __stdcall CommGeneralPage::Properties(IUnknown** iPrp)
{
	m_pProperties->QueryInterface(IID_IUnknown, (void**)iPrp);
	m_pProperties->Release();
	return S_OK;
}

HRESULT __stdcall CommGeneralPage::UnitTest()
{
	Radio* r = NULL;

	m_radios.Get(0, (IUnknown**)&r);
	r->SetFrequencyFromPreset(1);

	r = NULL;
	m_radios.Get(1, (IUnknown**)&r);
	r->Frequency(250.011);

	r = NULL;
	m_radios.Get(2, (IUnknown**)&r);
	r->Mode(RadioModeType::SATURN);
	return S_OK;
}

HRESULT __stdcall CommGeneralPage::Commit()
{
	BOOL bVal = FALSE;
	int nVal = 0;

	if (!m_pProperties->HasChanged()) // nothing to commit
		return S_OK;

	if (m_pIOSDevice == NULL)
	{
		m_pProperties->TurnOffChanged();
		return S_OK;
	}

	m_pBackupAudio->GetAsBool(&bVal);
	
	if (bVal == TRUE)
		m_pIOSDevice->SetAudioMode(AudioMode::AUDIO_MODE_BACKUP);
	else
		m_pIOSDevice->SetAudioMode(AudioMode::AUDIO_MODE_NORMAL);

	
	m_pSelectAuralCues->GetAsInt(0, &nVal);
	m_pIOSDevice->AuralCues(nVal);

	m_pAuralCueEnabled->GetAsBool(&bVal);

	if (bVal == TRUE)
		m_pSelectAuralCues->GetAsInt(0, &nVal);
	else
		nVal = 0;

	m_pIOSDevice->AuralCuesVolume(nVal);

	m_pIntercomEnabled->GetAsBool(&bVal);

	if (bVal == TRUE)
		m_pIntercomVolume->GetAsInt(0, &nVal);
	else
		nVal = 0;

	m_pIOSDevice->IntercomVolume(nVal);
	m_pProperties->TurnOffChanged();
	return S_OK;
}

void CommGeneralPage::Update()
{
	PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_AURAL_CUES_COCKPIT_SELECT);

	if (m_pIOSDevice == NULL)
		return;
	
	Radio* r = NULL;
	

	if (m_pMatchingOwnship != NULL)
	{
		DISNetwork& ownDev = m_pMatchingOwnship->GetDISNetwork();

		while (m_radios.ForEach((IUnknown**)&r) == S_OK)
		{
			r->Update(ownDev);
		}
	}	
}

void CommGeneralPage::Command(const wchar_t* szCmd)
{
	m_pProperties->Command(szCmd);

}

wstring  CommGeneralPage::GetReturnString()
{
	wstring ret;
	StringInf iStr;
	m_pProperties->GetReturnString(iStr);

	ret = (const wchar_t*)iStr;
	return ret;
}
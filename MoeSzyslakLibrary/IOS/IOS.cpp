#include "pch.h"
#include "IOS.h"

IosAssignRolesPage::IosAssignRolesPage()
{

}

IosAssignRolesPage::~IosAssignRolesPage()
{

}






IosChooseSfesPage::IosChooseSfesPage()
{

}

IosChooseSfesPage::~IosChooseSfesPage()
{

}

void IosChooseSfesPage::Update()
{
    m_IcDescription = L"File: " + m_scenarioPath;
    // m_IcDescription += L"\nAVSAF Assets: " + m_avsaf;
}








IosEstablishWizard::IosEstablishWizard()
{

}

IosEstablishWizard::~IosEstablishWizard()
{

}

int IosEstablishWizard::DoModal(wstring scenarioPath)
{
    NetworkDevices& nDev = NetworkDevices::GetNetworkDevice();
    RemoteDeviceModelF16* pF16 = NULL;
    wstring sScenarioPath;
      

    AVSAF(L"AFSAF 1");

    m_IosChooseSfesPage.ScenarioPath(scenarioPath);

    m_IosChooseSfesPage.Update();

    m_IosAssignRolesPage.SteamGauges(L"steam gauge");

    return 0;

}



LocalDeviceModelIos::LocalDeviceModelIos()
{
    PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_OPERATIONAL_MODE_IOS);
    PDU pduAuralCue(DatumIDAudio::DATUM_ID_FIXED_AUDIO_AURAL_CUES_COCKPIT_SELECT);

    pdu.Value(AudioMode::AUDIO_MODE_NORMAL);
    m_disNetwork.AddPDU(pdu);
    m_mode = SimModeTransition::TO_STANDBY;

    m_nAuralCues = -1;
    AuralCues(0);

    m_nAuralVolume = 0;
    AuralCuesVolume(50);

    m_nIntercomVolume = 0;
    IntercomVolume(50);
    m_pScenario = NULL;
}

LocalDeviceModelIos::~LocalDeviceModelIos()
{

}

void LocalDeviceModelIos::SetAudioMode(AudioMode mde)
{
    if (m_AudioMode == mde)
        return;

    m_AudioMode = mde;

    PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_OPERATIONAL_MODE_IOS);

    pdu.Value(mde);
    m_disNetwork.AddPDU(pdu);
}

void LocalDeviceModelIos::AuralCues(int nAuralCues)
{
    if (m_nAuralCues == nAuralCues)
        return;

    m_nAuralCues = nAuralCues;

    PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_AURAL_CUES_COCKPIT_SELECT);

    pdu.Value(m_nAuralCues);
    m_disNetwork.AddPDU(pdu);

}

void LocalDeviceModelIos::AuralCuesVolume(int nAuralCuesVol)
{
    if (m_nAuralVolume == nAuralCuesVol)
        return;

    m_nAuralVolume = nAuralCuesVol;

    PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_VOLUME_ENVIRONMENTAL);
    pdu.Value(m_nAuralVolume);
    m_disNetwork.AddPDU(pdu);
}

void LocalDeviceModelIos::IntercomVolume(int nIntercomVolume)
{
    if (m_nIntercomVolume == nIntercomVolume)
        return;

    m_nIntercomVolume = nIntercomVolume;

    PDU pdu(DatumIDAudio::DATUM_ID_FIXED_AUDIO_VOLUME_INTERCOM);
    pdu.Value(m_nIntercomVolume);
    m_disNetwork.AddPDU(pdu);

}






RemoteDeviceModelF16::RemoteDeviceModelF16()
{
    m_cRef = 1;
    m_deviceName = L"F16 Sim #1";
    m_state = SimState::STATE_READY;

    PDU pdu(OwnershipStatusDatumID);
    pdu.Value(m_state);
    m_disNetwork.AddPDU(pdu);

}

RemoteDeviceModelF16::~RemoteDeviceModelF16()
{

}

HRESULT __stdcall RemoteDeviceModelF16::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

ULONG __stdcall RemoteDeviceModelF16::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall RemoteDeviceModelF16::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
    }

    return m_cRef;
}

void RemoteDeviceModelF16::Update()
{
    PDU pdu(OwnershipStatusDatumID);
    m_disNetwork.GetPDU(OwnershipStatusDatumID, pdu);

    int n = pdu.GetIntValue();

    if (n >= 0 && n < SimState::STATE_NUM_STATES)
        m_state = (SimState)n;    

}


NetworkDevices* NetworkDevices::m_pTheNetworkDevices = NULL;

NetworkDevices::NetworkDevices()
{

}

NetworkDevices::~NetworkDevices()
{

}

NetworkDevices& NetworkDevices::GetNetworkDevice()
{
    if (m_pTheNetworkDevices == NULL)
        m_pTheNetworkDevices = new NetworkDevices();

    return *m_pTheNetworkDevices;
}

void NetworkDevices::Add(RemoteDeviceModelF16& f16)
{
    m_devices.Add(&f16, f16.DeviceName().c_str(), 0);

}

bool NetworkDevices::ForEach(RemoteDeviceModelF16** pF16)
{
    return (m_devices.ForEach((IUnknown**)pF16) == S_OK);

}






IOS::IOS()
{
    m_cRef = 1;
    m_pF16Device = new RemoteDeviceModelF16();
    CommGeneralPage* pComm = (CommGeneralPage*)((IUnknown*)m_iCommGeneralPage);

    m_iosDevice.Mode(SimModeTransition::TO_TRAIN);
    pComm->SetLocalDevice(&m_iosDevice);
    
    VLVariable* v = m_properties.NewVariable(L"Comm General Page");
    v->SetInterface(m_iCommGeneralPage, CommsInf::ClassID);

    NetworkDevices& nDev = NetworkDevices::GetNetworkDevice();
    nDev.Add(*m_pF16Device);

    m_pScenario = new Scenario();
    v = m_properties.NewVariable(L"scenario");
    v->SetInterface(m_pScenario, 0);
}

IOS::~IOS()
{
    m_pF16Device->Release();  
    m_pScenario->Release();
}

ULONG IOS::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall IOS::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall IOS::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == IOS_IID)
    {
        *ppvObj = static_cast<IOSInf::IIOS*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall IOS::Properties(IUnknown** iProp)
{
    *iProp = NULL;

    m_properties.QueryInterface(IID_IUnknown, (void**)iProp);
    m_properties.Release();
    return S_OK;
}

HRESULT __stdcall IOS::Update()
{
    m_pF16Device->Update();
    return S_OK;
}

HRESULT __stdcall IOS::UnitTest()
{
    VariableInfCollection iPrp;
    wstring cmd;

    CommGeneralPage* pComm = (CommGeneralPage*)((IUnknown*)m_iCommGeneralPage);

    Command(L"\"Comm General Page\" set \"Backup Audio Enabled\" true");

    m_iCommGeneralPage->Properties(iPrp);
    cmd = L"\"Comm General Page\" set \"Cockpit Selection\" \"";
    cmd += m_pF16Device->DeviceName() + L"\"";
    Command(cmd.c_str());

    Command(L"\"Comm General Page\" set \"Aural Cues Enabled\" true");
    
    iPrp.Set(L"Aural Cues Volume", L"50");
    iPrp.SetBool(L"Intercom Enabled", true);
    iPrp.Set(L"Intercom Volume", L"51");

    pComm->SetMatchingOwnship(m_pF16Device);

    m_iCommGeneralPage->UnitTest();
    m_iCommGeneralPage->Commit();

    pComm->Update();    

    VariableCollection& iScenPrp = m_pScenario->Properties();

    iScenPrp.Set(L"IC File Path", L"T:\\vloweM74-3-021\\f16_IOS\\data\\Server\\Scenarios\\IC_Sets\\Nellis_#1\\Nellis_#1.sgs", VLVariable::VAR_TYPE::TYPE_STRING);

    m_pScenario->SelectedTrainer(m_pF16Device);
    Command(L"scenario set record true");

    EstablishExercise();
    return S_OK;
}

HRESULT __stdcall IOS::Command(const wchar_t* szCmd)
{
    VLStringCollection cmds;
    
    cmds.Split(szCmd, L' ');
    cmds.ToLower(0);
    
    if (cmds.Compare(0, L"comm general page"))
    {
        CommGeneralPage* pComm = (CommGeneralPage*)((IUnknown*)m_iCommGeneralPage);
        pComm->Command(cmds.SubString(1, L' ').c_str());
        return S_OK;
    }
    else if (cmds.Compare(0, L"scenario"))
    {
        m_pScenario->Command(cmds.SubString(1, L' ').c_str());
        return S_OK;
    }
    else
        return E_FAIL;
}

HRESULT __stdcall IOS::EstablishExercise()
{
    VariableCollection& iPrp = m_pScenario->Properties();
    wstring path = iPrp.Get(L"IC File Path");

    if (!VLFile::FileExists(path))
        return E_FAIL;

    IosEstablishWizard initWizard;
    int nInitResult = IDOK;  

    m_pScenario->TES(L"TES 1");

    nInitResult = initWizard.DoModal(path);
    return S_OK;
}


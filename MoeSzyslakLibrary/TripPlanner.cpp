#include "pch.h"
#include "TripPlanner.h"
#include "tinyxml2.h"

using namespace tinyxml2;


Stop::Stop()
{
    m_cRef = 1;
	m_pStart = m_properties.NewVariable(L"Start Location");
	m_pStart->SetString(L"New Stop");
	m_pStart->CanBeEmpty(false);

    m_distance = 0;
    m_travelTime = 0;
}

Stop::~Stop()
{
}

ULONG Stop::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall Stop::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall Stop::QueryInterface(REFIID riid, LPVOID* ppvObj)
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

HRESULT __stdcall Stop::Properties(IUnknown** iProp)
{
    m_properties.QueryInterface(IID_IUnknown, (void**)iProp);
	m_properties.Release();
	return S_OK;
}



TripPlanner::TripPlanner()
{
    m_cRef = 1;
    m_pName = m_properties.NewVariable(L"Trip Name");
    m_pName->SetString(L"New Trip");
    m_pName->CanBeEmpty(false);

    m_MPG = 0.0;
}
    

TripPlanner::~TripPlanner()
{
    

}

ULONG TripPlanner::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG __stdcall TripPlanner::Release()
{
    m_cRef--;

    if (0 == m_cRef)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

HRESULT __stdcall TripPlanner::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        AddRef();
        return NOERROR;
    }
    else if (riid == TripPlannerInf::m_iid)
    {
        *ppvObj = static_cast<TripPlannerInf::ITRIPPLANNER*>(this);
        AddRef();
        return NOERROR;
    }
    else
        return E_NOINTERFACE;
}

HRESULT __stdcall TripPlanner::Properties(IUnknown** iProp)
{
    m_properties.QueryInterface(IID_IUnknown, (void**)iProp);
	return S_OK;
}

HRESULT __stdcall TripPlanner::Command(const wchar_t* szCmd)
{
    HRESULT hr = E_FAIL;
    VLStringCollection cmds;
	IUnknown* iunk = NULL;
	int nCnt = 0;

    switch (szCmd[0])
    {
    case L'A':
    case L'a':
        cmds.Split(szCmd, L' ');
		cmds.ToLower(0);

        if (cmds.Compare(0, L"addstop"))
        {
            hr = AddStop(&iunk);
            m_stops.Count(&nCnt);
			m_iRetStr->Set(std::to_wstring(nCnt).c_str());
        }
        break;
    }

    return hr;

}

HRESULT __stdcall TripPlanner::GetReturnString(IUnknown** iStr)
{
	*iStr = m_iRetStr;
    return S_OK;
}

HRESULT __stdcall TripPlanner::UnitTest()
{
    ReadXML();
	VariableInfCollection iPrp;
    Stop* pStop = GetStop(0);

	pStop->Properties(iPrp);
    iPrp.Set(L"Start Location", L"Home");
	pStop->Distance(115);
	pStop->TravelTime(120);

    m_MPG = 30.1;

    pStop = GetStop(1);
    pStop->Properties(iPrp);
    iPrp.Set(L"Start Location", L"Valero");
    pStop = NULL;

    while (m_stops.ForEach((IUnknown**)& pStop) == S_OK)
    {
		double fuelUsed = (double)pStop->Distance() / m_MPG;
        pStop->FuelUsed(fuelUsed);
        pStop->Print();
    }

    WriteXML();
	return S_OK;
}

HRESULT __stdcall TripPlanner::AddStop(IUnknown** iStop)
{
	Stop* pStop = new Stop();
	m_stops.Add(pStop, L"", 0);
	pStop->QueryInterface(IID_IUnknown, (void**)iStop);

    pStop->Release();
    pStop->Release();
	return S_OK;
}

void TripPlanner::WriteXML()
{
    tinyxml2::XMLDocument doc;
	Stop* pStop = NULL;

    // Root element
    XMLElement* root = doc.NewElement("Trip");
    doc.InsertFirstChild(root);

    // trip name
    XMLElement* stop1 = doc.NewElement("name");
    wstring str;
    string asc;

    str = m_pName->GetString();
	asc = string(str.begin(), str.end());
    stop1->SetText(asc.c_str());
    root->InsertEndChild(stop1);

    while (m_stops.ForEach((IUnknown**)&pStop) == S_OK)
    {
        stop1 = doc.NewElement("stop");
        root->InsertEndChild(stop1);
    }

    // Save to file
    XMLError e = doc.SaveFile("test_file.xml");
    if (e == XML_SUCCESS)
        printf("XML exported to test_file.xml\n");
    else
        printf("Error writing XML\n");
}

void TripPlanner::ReadXML()
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile("test_file.xml") != XML_SUCCESS)
        return;

    XMLElement* root = doc.FirstChildElement("Trip");
    if (!root)
        return;

    XMLElement* stop = root->FirstChildElement("name");

    string asc = stop->GetText();
	wstring str(asc.begin(), asc.end());
	m_pName->SetString(str.c_str());

    m_stops.Clear();
    stop = root->FirstChildElement("stop");

    while (stop != NULL)
    {
        Stop* pStop = new Stop();
        m_stops.Add(pStop, L"", 0);
        pStop->Release();
        stop = stop->NextSiblingElement("stop");
    }
    
}

Stop* TripPlanner::GetStop(UINT ndx)
{
    Stop* pStop = NULL;
    m_stops.Get(ndx, (IUnknown**)&pStop);
	return pStop;
}
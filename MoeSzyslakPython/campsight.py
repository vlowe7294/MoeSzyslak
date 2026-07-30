import ctypes
import comtypes

from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_int32, c_uint, c_void_p, c_double, byref, HRESULT, c_wchar_p, create_unicode_buffer, cast
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER, BSTR
from flask import Flask, jsonify, render_template, request
from testing import Testing, ITESTING

CAMPAREA_IID = GUID("{764BAFFA-9281-4610-8683-E0B8B0841D0F}")

class ICAMPAREA(IUnknown):
    _iid_ = CAMPAREA_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "GetName",
            (["out, retVal"], CPOINTER(BSTR), "iTester")
        ),

        COMMETHOD(
            [], HRESULT, "SetName",
                  (['in'], c_wchar_p, "strNme"),
        ),        
    ]
    
class CampArea():
    def __init__(self, iunk):
        self._iunk = iunk
        unk = ctypes.cast(self._iunk, CPOINTER(IUnknown))
        self._iCampArea = unk.QueryInterface(ICAMPAREA)

    @property
    def Name(self):
        txt_bstr = BSTR()
        self._iCampArea.GetName(comtypes.byref(txt_bstr))
        return txt_bstr.value
    

CAMPSIGHT_IID = GUID("{583AFD09-699D-42C2-98FE-753E75422F16}")

class ICAMPSIGHT(IUnknown):
    _iid_ = CAMPSIGHT_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "GetTester",
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iTester")
        ),

        COMMETHOD(
            [], HRESULT, "AddSite",
                  (['in'], c_wchar_p, "szArea"),
                  (['in'], c_wchar_p, "szSite"),
                  (['in'], c_double, "lat"),
                  (['in'], c_double, "lon")
                  ),

        COMMETHOD(
            [], HRESULT, "GetArea",
                  (['in'], c_int32, "ndx"),
                  (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iArea"),
                  ),
    ]

class CampSight():
    class_id = 929959

    def __init__(self):
        MoeSzyslakLibrary.VerifyLibrary()
        unk_ptr = c_void_p()
        MoeSzyslakLibrary.CreateMoeSzyslakInterface(CampSight.class_id, byref(unk_ptr))

        unk = ctypes.cast(unk_ptr.value, CPOINTER(IUnknown))
        self._iCampSight = unk.QueryInterface(ICAMPSIGHT)

        unk_ptr = CPOINTER(IUnknown)()
        self._iCampSight.GetTester(byref(unk_ptr))
        self._tester = Testing(unk_ptr.QueryInterface(ITESTING))

        self._campAreas = []

    def AddSite(self, strArea, strSite, lat, lon):
        MoeSzyslakLibrary.check_hresult(self._iCampSight.AddSite(strArea, strSite, lat, lon))

    def Update(self):
        i = 0;
       
        iArea = CPOINTER(IUnknown)()       
        self._campAreas.clear()
        self._iCampSight.GetArea(0, byref(iArea))
        
        while iArea:
            ca = CampArea(iArea)
            self._campAreas.append(ca)
            i = i + 1

            iArea = CPOINTER(IUnknown)()     
            self._iCampSight.GetArea(i, byref(iArea))

        print(f"_campAreas = {self._campAreas}")
            

    @property
    def theTester(self):
        return self._tester


    def UnitTest():
        cs = CampSight()
        tst = cs.theTester

        try:
            tst.DebugLevel = Testing.DEBUG_CRITICAL
        
            cs._tester.Message("Camp Sight Unit Test", Testing.DEBUG_INFO, "campsight")
            cs.AddSite("Dinosaur Valley State Park", "Laham Mill #14", 32.251301564676666, -97.8112404606453)
            cs.Update()
            tst.Verify(cs._campAreas[0].Name == "Dinosaur Valley State Park", "Area name mismatch", Testing.DEBUG_CRITICAL, "campsight")

            print(f"Name = {cs._campAreas[0].Name}")
        except Exception as e:
            print(e)
            tst.Verify(False, f"Exception thrown during unit test: {e}", Testing.DEBUG_CRITICAL, "campsight")

        tst.Report()
        print("areas set to none")
        cs = None
        
app = Flask(__name__)
app.CampSight = None

@app.route("/campsight", methods=["GET", "POST"])
def campsight():
    if app.CampSight is None:
        app.CampSight = CampSight()

    if request.method == "POST":
        app.CampSight.AddSite(request.form["area"], request.form["name"], float(request.form["lat"]), float(request.form["lon"]))
        
    return render_template("campsight.html")

@app.route("/api/sites", methods=["POST"])
def api_add_site():
    data = request.json
    app.CampSight.AddSite(data["area"], data["name"], data["lat"], data["lon"])
    return jsonify({"status": "ok"})

def test_campsight():
    CampSight.UnitTest()

def test_tester():
    cs = CampSight() 
    Testing.UnitTest(cs)


if __name__ == "__main__":  
    test_campsight()





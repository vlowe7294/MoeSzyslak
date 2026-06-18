import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, c_double, byref, HRESULT, c_wchar_p, create_unicode_buffer, cast
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER
from flask import Flask, jsonify, render_template, request
from testing import Testing, ITESTING

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
        self._tester = Testing(unk_ptr.QueryInterface(ITESTING));

    def AddSite(self, strArea, strSite, lat, lon):
        MoeSzyslakLibrary.check_hresult(self._iCampSight.AddSite(strArea, strSite, lat, lon))

    def theTester(self):
        return self._tester;


    def UnitTest(self):
        print("Testing CampSight...")
        self._tester.Message("Camp Sight Unit Test")
        self.AddSite("Dinosaur Valley State Park", "Laham Mill #14", 32.251301564676666, -97.8112404606453);
        
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
    cs = CampSight() 
    cs.UnitTest()

def test_tester():
    cs = CampSight() 
    Testing.UnitTest(cs)


if __name__ == "__main__":  
    test_tester()





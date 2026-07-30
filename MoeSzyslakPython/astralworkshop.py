import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, byref, HRESULT, c_wchar_p, create_unicode_buffer, cast, c_int32
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER, BSTR
from flask import Flask, jsonify, render_template, request
from pathlib import Path
import comtypes
from testing import Testing, ITESTING

CREATURE_IID = GUID("{8346DA8D-9EFD-436A-9545-8D324DE0E43E}")

class ICREATURE(IUnknown):
    _iid_ = CREATURE_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "Tick",
            (["in"], c_int32, "nSec"),
        ),

        COMMETHOD(
            [], HRESULT, "SetArea",
            (["in"], CPOINTER(IUnknown), "iArea"),
        ),
    ]

class Creature:
    CLASS_NONE = 0
    CLASS_WARRIOR = 1
    CLASS_MAGE = 2
    CLASS_ROGUE = 3
    CLASS_MAX = 4    

class Character(Creature):
    BACKGROUND_NONE = 0
    BACKGROUND_NOBLE = 1
    BACKGROUND_PEASANT = 2
    BACKGROUND_OUTLAW = 3
    BACKGROUND_MAX = 4

    def __init__(self, iunk):
        self._iunk = iunk
        self._iCreature = iunk.QueryInterface(ICREATURE)

    def SetArea(self, ar):
        MoeSzyslakLibrary.check_hresult(self._iCreature.SetArea(ar.theUnknown())) 
        
LOCATION_IID = GUID("{7D5339E3-2CA0-4EDB-8277-6140AC1F5F05}")

class ILOCATION(IUnknown):
    _iid_ = LOCATION_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "AddNeighbor",
            (["in"], CPOINTER(IUnknown), "iNeighbor"),
            (["in"], c_int32, "travelTimeSec"),
        ),

        COMMETHOD(
            [], HRESULT, "AddEncounter",
            (['in'], c_wchar_p, "strTag"),
            (["in"], c_int32, "dif"),
            (["in"], c_int32, "nMax"),
        ),

        COMMETHOD(
            [], HRESULT, "AddPlaceable",
            (['in'], c_wchar_p, "strName"),
            (["in"], c_wchar_p, "strTag"),
        ),

        COMMETHOD(
            [], HRESULT, "GetProperties",
            (["in"], CPOINTER(CPOINTER(IUnknown)), "iPrp"),
        ),
    ]

class Location:
    DIFFICUTLY_VERY_EASY = 0
    DIFFICUTLY_EASY = 1
    DIFFICUTLY_NORMAL = 2
    DIFFICUTLY_HARD = 3
    DIFFICUTLY_IMPOSSIBLE = 4

    def __init__(self, iunk):
        self._iunk = iunk
        self._iLocation = iunk.QueryInterface(ILOCATION)

        iPrp = CPOINTER(IUnknown)() 
        self._iLocation.GetProperties(byref(iPrp))
        self._name = ''

    def AddNeighbor(self, loc, travelTimeSec):
        self._iLocation.AddNeighbor(loc.theUnknown(), travelTimeSec)

    def AddEncounter(self, strTag, dif, nMax):
        self._iLocation.AddEncounter(strTag, dif, nMax)

    def theUnknown(self):
        return self._iunk

    @property
    def Name(self):
        return self._name

    @Name.setter
    def Name(self, strName):
        self._name = strName

    



AREA_IID = GUID("{9E39F153-0811-498B-B72A-5C810D0141A0}")

class IAREA(IUnknown):
    _iid_ = AREA_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "AddLocation",
            (["in"], CPOINTER(CPOINTER(IUnknown)), "iLoc"),
        ),
    ]

class Area:
    def __init__(self, iunk):
        self._iunk = iunk
        self._iArea = iunk.QueryInterface(IAREA)
        self._locations = []

    def AddLocation(self):
        iLoc = CPOINTER(IUnknown)() 
        self._iArea.AddLocation(byref(iLoc))
        l = Location(iLoc)

        self._locations.append(l)
        return l

    def theUnknown(self):
        return self._iunk

    def UnitTest(self, tst):
         tst.TestData("location a", "southeast edge")
         tst.TestData("location b", "debris")
         tst.Message("Area Unit Test", Testing.DEBUG_INFO, "Area")

         la = self.AddLocation()
         la.Name = tst.TestData("location a")

         lb = self.AddLocation()
         lb.Name = "a fog bank"
         la.AddNeighbor(lb, 60)
         lb.AddEncounter("NW_UndeadAll", Location.DIFFICUTLY_NORMAL, 8)
        



ASTRALWORKSHOP_IID = GUID("{F43BA252-2FCA-41A2-9CFA-22A5D87C584C}")

class IASTRALWORKSHOP(IUnknown):
    _iid_ = ASTRALWORKSHOP_IID
    _methods_ = [
        COMMETHOD(
            [], HRESULT, "NewArea",
            (["in"], CPOINTER(CPOINTER(IUnknown)), "iArea"),
            (["in"], CPOINTER(ctypes.c_int), "ndx"),
            (['in'],  c_wchar_p, "szName"),
            (['in'],  c_uint, "nDanger"),
        ),
        COMMETHOD(
            [], HRESULT, "Export",
            (['in'],  c_wchar_p, "strPath"),
        ),
        COMMETHOD(
            [], HRESULT, "Save",
            (['in'],  c_wchar_p, "strPath"),
        ),
        COMMETHOD(
            [], HRESULT, "NewCharacter",
            (["in"], CPOINTER(CPOINTER(IUnknown)), "iCharacter"),
            (['in'],  c_wchar_p, "strName"),
            (['in'],  c_int32, "nClass"),
            (['in'],  c_int32, "nBackground"),
        ),
        COMMETHOD(
            [], HRESULT, "Heartbeat",
            (['in'],  c_int32, "nSec"),
        ),
        COMMETHOD(
            [], HRESULT, "GetTester",
            (["in"], CPOINTER(CPOINTER(IUnknown)), "iTst"),
        ),

        COMMETHOD(
            [], HRESULT, "GetAreaNames",
            (['out, retVal'], CPOINTER(BSTR), "areaList"),
        ),
    ]

class AstralWorkshop():
    class_id = 133671 
    _sb = create_unicode_buffer(1024)

    def __init__(self):
        MoeSzyslakLibrary.VerifyLibrary()
        unk_ptr = c_void_p()
        MoeSzyslakLibrary.CreateMoeSzyslakInterface(AstralWorkshop.class_id, byref(unk_ptr))

        unk = ctypes.cast(unk_ptr.value, CPOINTER(IUnknown))
        self._iAstralWorkshop = unk.QueryInterface(IASTRALWORKSHOP)

        iTst = CPOINTER(IUnknown)()   
        self._iAstralWorkshop.GetTester(byref(iTst))
        self._test = Testing(iTst)   

        self.state = "normal"        # normal | creating_character
        self.char_data = {}          # temporary storage during creation
        self._areas = []

    def Run(self):
        bExit = False
        cmd = input("Astral Workshop -- Enter command:  ")

        while bExit is False:
            prmpt = self.Command(cmd)

            if cmd == "exit":
                bExit = True
            else:
                cmd = input(f"{prmpt}  ")

    def NewArea(self, nme, nDnger):
        iArea = CPOINTER(IUnknown)()   
        ndx = ctypes.c_int()
        MoeSzyslakLibrary.check_hresult(self._iAstralWorkshop.NewArea(byref(iArea), ctypes.byref(ndx), nme, nDnger))

        a = Area(iArea.value)
        self._areas.append(a)
        return a

    def Export(self):
        strPath = str(Path(__file__).parent/"astralworkshop.xml")
        MoeSzyslakLibrary.check_hresult(self._iAstralWorkshop.Export(strPath))

    def Save(self):
        strPath = str(Path(__file__).parent/"astralworkshop.dat")
        MoeSzyslakLibrary.check_hresult(self._iAstralWorkshop.Save(strPath))

    def NewCharacter(self, strName, nClass, bkgd):
        iChrctr = CPOINTER(IUnknown)()
        self._iAstralWorkshop.NewCharacter(byref(iChrctr), strName, nClass, bkgd)
        return Character(iChrctr)

    def Heartbeat(self, nSec):
        self._iAstralWorkshop.Heartbeat(nSec)

    def Areas(self):
        return self._areas

    def GetAreaNames(self):
        txt_bstr = BSTR()
        self._iAstralWorkshop.GetAreaNames(comtypes.byref(txt_bstr))
        return txt_bstr.value.split("\n")


    def UnitTest(self):
        try:
            self._test.Message("Astral Workshop unit test", Testing.DEBUG_INFO, "AstralWorkshop");
            a = self.NewArea("Whispering Woods", 2)
            self.NewArea("Graveyard", 3)

            a.UnitTest(self._test)
            self.GetAreaNames()            
            
            c = self.NewCharacter("Gert Addams", Creature.CLASS_WARRIOR, Character.BACKGROUND_PEASANT)
            c.SetArea(a)
            self.Heartbeat(60)

            self.Export()
            self.Save()
            self._test.Message("Test complete", Testing.DEBUG_INFO, "Astral Workshop")
        except Exception as e:
            print(e)

        self._test.Report()

def test_astral_workshop():
    aw = AstralWorkshop() 
    aw.UnitTest()

app = Flask(__name__)
app.UserSvc = None
app.AstralWorkshop = None

@app.route("/astralworkshop/edit", methods=["GET", "POST"])
def astralworkshopEdit():
    try:
        areas = []
        if app.AstralWorkshop is None:
            app.AstralWorkshop = AstralWorkshop()

        if request.method == "POST":
            print(f"area_name = {request.form["area_name"]}")
            print(f"danger = {request.form["danger"]}")
            app.AstralWorkshop.NewArea(request.form["area_name"], int(request.form["danger"]))

            app.AstralWorkshop.Export()
            app.AstralWorkshop.Save()        
            areas = app.AstralWorkshop.GetAreaNames()

        return render_template("astralworkshop.html", areas=areas)

    except Exception as e:
        htm = "<h1>" + str(e) + "</h1>"

    return htm

@app.route("/astralworkshop/play", methods=["GET", "POST"])
def command():
    if app.AstralWorkshop is None:
            app.AstralWorkshop = AstralWorkshop()

    return render_template("astralplay.html")

if __name__ == "__main__":    
    #app.run(port=5000)
    test_astral_workshop()
    
    






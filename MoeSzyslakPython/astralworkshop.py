import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, byref, HRESULT, c_wchar_p, create_unicode_buffer, cast
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER
from flask import Flask, jsonify, render_template, request
from pathlib import Path


class Area:
    def __init__(self, iunk):
        self._iunk = iunk



ASTRALWORKSHOP_IID = GUID("{F43BA252-2FCA-41A2-9CFA-22A5D87C584C}")

class IASTRALWORKSHOP(IUnknown):
    _iid_ = ASTRALWORKSHOP_IID
    _methods_ = [        
        COMMETHOD(
            [], HRESULT, "Command",
            (["in"], c_wchar_p, "strCmd"),
            (["in"], c_wchar_p, "strRet"),
            (["in"], c_uint, "nLen"),   
        ),
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

        self.state = "normal"        # normal | creating_character
        self.char_data = {}          # temporary storage during creation
        self._areas = []

    def Command(self, text):

        buf_ptr = cast(AstralWorkshop._sb, c_wchar_p)
        hr = self._iAstralWorkshop.Command(text, buf_ptr, 1024)
        return AstralWorkshop._sb.value

        # Start character creation
        if text == "create character":
            self.char_data = {}
            return "Welcome to character creation.\nWhat is your character's name?"
        else:
            return "I don't know what to do with that command."

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
        self._areas.append(Area(iArea))

        print("Index:", ndx.value)
        print("Returned IUnknown:", iArea)

    def Export(self):
        strPath = str(Path(__file__).parent/"astralworkshop.xml")
        self._iAstralWorkshop.Export(strPath)

    def Save(self):
        strPath = str(Path(__file__).parent/"astralworkshop.dat")
        self._iAstralWorkshop.Save(strPath)


    def UnitTest(self):
        print("Testing Astral Workshop...")
        self.NewArea("Whispering Woods", 2)
        self.NewArea("Graveyard", 3)

        self.Export()
        self.Save()
        print("astralWorkshop tests passed.")

def test_astral_workshop():
    aw = AstralWorkshop() 
    aw.UnitTest()

app = Flask(__name__)
app.UserSvc = None
app.AstralWorkshop = None

@app.route("/astralworkshop/edit", methods=["GET", "POST"])
def astralworkshopEdit():
    try:
        if app.AstralWorkshop is None:
            app.AstralWorkshop = AstralWorkshop()

        if request.method == "POST":
            print(f"area_name = {request.form["area_name"]}")
            print(f"danger = {request.form["danger"]}")
            app.AstralWorkshop.NewArea(request.form["area_name"], int(request.form["danger"]))
            
            app.AstralWorkshop.Export()
            app.AstralWorkshop.Save()
        

        return render_template("astralworkshop.html")

    except Exception as e:
        htm = "<h1>" + str(e) + "</h1>"

    return htm

@app.route("/command", methods=["POST"])
def command():
    cmd = request.form["cmd"]
    print(f"Received command: {cmd}")
    return render_template("astralworkshop.html")

if __name__ == "__main__":    
    app.run(port=5000)
    #test_astral_workshop()
    
    






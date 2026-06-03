import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, byref, HRESULT, c_wchar_p, create_unicode_buffer, cast
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER
from flask import Flask, jsonify, render_template, request


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

        pass

    def UnitTest(self):
        print("Testing Astral Workshop...")

        response = self.Command("Create")
        print("Enter command:  Create")
        print(f"{response}  Rowan")

        response = self.Command("Rowan")
        print(f"{response}  1")

        response = self.Command("1")
        print(f"{response}  2")

        response = self.Command("2")
        print(f"{response}  exit")

        response = self.Command("exit")
        print(f"{response}")

        print("astralWorkshop tests passed.")

def test_astral_workshop():
    aw = AstralWorkshop() 
    #aw.UnitTest()
    aw.Run()

app = Flask(__name__)
app.UserSvc = None

@app.route("/astralworkshop", methods=["GET", "POST"])
def astralworkshop():
    try:
        return render_template("astralworkshop.html")

    except Exception as e:
        htm = "<h1>" + str(e) + "</h1>"

    return htm

if __name__ == "__main__":    
    #app.run(port=5000)
    test_astral_workshop()
    
    






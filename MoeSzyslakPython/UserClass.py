import time
import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, byref, HRESULT, c_wchar_p
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER
from flask import Flask, jsonify, render_template, request

__version__ = 1601

# Replace with your real IID
USER_IID = GUID("{3B1537E5-9E5D-4053-8510-1CEB51543F32}")

class IUSER(IUnknown):
    _iid_ = USER_IID
    _methods_ = [
        # HRESULT GetUser([in] const wchar_t* szName, [out] IUnknown** iUsr)
        COMMETHOD(
            [], HRESULT, "Login",
            (["in"], c_wchar_p, "szName"),
            (["in"], c_wchar_p, "szPwd"),
        ),
    ]

class User:    
    _classID = 325850 
    Version = __version__
    _masterUserList = dict()
    _bInit = False
    LogoutMinutes = 15

    def __init__(self, iUser):
        self._iUser = iUser;
        print(f"interface created = {self._iUser}")

    def Print(self):
        self._varName.Print()
        print("Password:\t", self._password)
        print("Is logged in:\t", self._bIsLoggedIn)
        print("Is Admin:\t", self._isAdmin)
        print("Last Active:\t", time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(self._lastActive)))

    def InitMasterList():
        if not User._bInit:
            User._bInit = True
            u = User()
            
            u._name = 'Vaughn'

            u._password = 'ZFyZH8DuKemv'
            u._isAdmin = True
            User._masterUserList['Vaughn'] = u

    def Login(self):

        if User._masterUserList.get(self._name) is None:
            return

        if User._masterUserList[self._name].Compare(self) is True:
            self._isAdmin = User._masterUserList[self._name]._isAdmin
            self._bIsLoggedIn = True
            self._lastActive = time.time() 


    def Compare(self, usr):

        if self._name != usr._name:
            return False

        if self._password != usr._password:
            return False

        return True

    def AddToMasterList(self, strTag, strPassword):
        if self._bIsLoggedIn is False: 
            raise Exception("User must be logged in to add users to the master list")

        if self._isAdmin is False: 
            raise Exception("only administrators can add users to the master list")

        if not isinstance(strTag, str) or len(strTag) == 0:
            raise TypeError("new user name must be a string and not empty")

        if not isinstance(strPassword, str) or len(strPassword) == 0:
            raise TypeError("new password must be a string and not empty")

        if User._masterUserList.get(strTag) is None:
            iUsr = User()
            iUsr.LoginName = strTag
            iUsr.Password = strPassword
            User._masterUserList[strTag] = iUsr
        else:
            raise Exception("user already exists")

    def UnitTest(tst):
        tst.Message(f"User Unit Test user.py version 1.3.6.{User.Version - 1530}")
        usr = User()

        usr.LoginName = tst.TestValue("Login Name")
        usr.Password = tst.TestValue("Password")
        usr.Login()

        if tst.TestValue("Valid Login") is True:
            tst.Verify(usr.IsLoggedIn, "User failed to log in")
        else:
            tst.Verify(usr.IsLoggedIn == False, "User should not have logged in")

        tst.Verify(usr.IsAdmin, "User is not an admin")
        
        usr.Print()

        tst.Verify(usr.LoginName == tst.TestValue("Login Name"), "Login name does not match expected value")
        tst.Verify(usr.Password == tst.TestValue("Password"), "Password does not match expected value")


    @property
    def LoginName(self):
        return self._name

    @LoginName.setter
    def LoginName(self, new_value):
        if not isinstance(new_value, str):
            raise TypeError("LoginName must be a string")

        self._name = new_value

    @property
    def Password(self):
        return self._password

    @Password.setter
    def Password(self, new_value):
        if not isinstance(new_value, str):
            raise TypeError("Password must be a string")

        self._password = new_value

    @property
    def IsLoggedIn(self):
        if self._lastActive is None:
            return self._bIsLoggedIn

        elapsed = time.time() - self._lastActive

        if elapsed > User.LogoutMinutes * 60:
            self._bIsLoggedIn = False

        return self._bIsLoggedIn

    @property
    def IsAdmin(self):
        return self._isAdmin

# Replace with your real IID
USERSERVICE_IID = GUID("{39663C08-D7C0-4CB2-B4F0-0E1F2445A5B0}")


class IUSERSERVICE(IUnknown):
    _iid_ = USERSERVICE_IID
    _methods_ = [
        # HRESULT GetUser([in] const wchar_t* szName, [out] IUnknown** iUsr)
        COMMETHOD(
            [], HRESULT, "GetUser",
            (["in"], c_wchar_p, "szName"),
            (["out, retVal"], CPOINTER(CPOINTER(IUnknown)), "iUsr"),
        ),

        COMMETHOD([], HRESULT, "UnitTest"),
    ]

class UserService:
    class_id = 241805


    def __init__(self):
        MoeSzyslakLibrary.VerifyLibrary()
        unk_ptr = c_void_p()
        MoeSzyslakLibrary.CreateMoeSzyslakInterface(UserService.class_id, byref(unk_ptr))

        unk = ctypes.cast(unk_ptr.value, CPOINTER(IUnknown))
        self.iUserService = unk.QueryInterface(IUSERSERVICE)

    def UnitTest(self):
        #hr = self.iUserService.UnitTest()
        #MoeSzyslakLibrary.check_hresult(hr, "UnitTest failed")
        usr = self.GetUser("Vaughn")

    def GetUser(self, name):
        unk_ptr = CPOINTER(IUnknown)()
        hr = self.iUserService.GetUser(name, byref(unk_ptr))        
        MoeSzyslakLibrary.check_hresult(hr, "GetUser failed") 

        if unk_ptr.value is None:
            return None
        else:
            return User(unk_ptr.QueryInterface(IUSER))

        

        

def test_user_test():
    us = UserService() 
    us.UnitTest()

# ----------------------------------------------------------------------
# Flask App
# ----------------------------------------------------------------------

app = Flask(__name__)
app.UserSvc = None

@app.route("/login", methods=["GET", "POST"])
def login():
    rslt = ''

    htm = ''
    nme = ''

    try:
        if app.UserSvc is None:
            app.UserSvc = UserService()

        if  request.method == "POST":
            nme = request.form.get("Login Name")
            usr = app.UserSvc.GetUser(nme)

            if usr is None:
                print(f"Login failed for user: {nme}")
            else:
                print(f"Login succeeded for user: {nme}")

        return render_template("login.html", LoginName=nme)

    except Exception as e:
        htm = "<h1>" + str(e) + "</h1>"

    return htm
    
    
    
if __name__ == "__main__":    
    app.run(port=5000)
    #test_user_test()
    


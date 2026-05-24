import time
import ctypes
from MoeSzyslakPython import MoeSzyslakLibrary
from ctypes import c_uint, c_void_p, byref, HRESULT, c_wchar_p
from comtypes import IUnknown, GUID, COMMETHOD, POINTER as CPOINTER
from flask import Flask, jsonify

__version__ = 1601

class User:    
    _classID = 325850 
    Version = __version__
    _masterUserList = dict()
    _bInit = False
    LogoutMinutes = 15

    def __init__(self):
        User.InitMasterList()
        self._name = 'New User'
        self._password = 'xxxxx'
        self._bIsLoggedIn = False
        self._isAdmin = False
        self._lastActive = None  

        self._properties = VariableCollection()

        self._varName = self._properties.NewVariable("Login Name")
        self._varName.SetString(self._name)

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
        self.GetUser("Vaughn")

    def GetUser(self, name):
        iUser = CPOINTER(IUnknown)()
        self.iUserService.GetUser(name, byref(iUser))

        if not iUser:
            print(f"GetUser did not find user {name} result is {iUser}")
        else:
            print(f"GetUser succeeded for {name} result is {iUser}")

        

def test_user_test():
    us = UserService() 
    us.UnitTest()

# ----------------------------------------------------------------------
# Flask App
# ----------------------------------------------------------------------

app = Flask(__name__)

@app.route("/userservice/unit_test")
def unit_test():
    rslt = ''

    try:
        us = UserService() 
        us.UnitTest()        
        rslt = "Unit test succeeded"
    except Exception as e:
        rslt = str(e)
        print(e)

    return jsonify({"Result:  ": rslt})
    
    
    
if __name__ == "__main__":    
    #app.run(port=5000)
    test_user_test()
    


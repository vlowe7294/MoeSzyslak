import testing
from testing import T, TestRunner, VLVariable
import time

__version__ = 1587

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

        self._varName = VLVariable()
        self._varName.DisplayName = "Login Name"
        self._varName.SetString(self._name)

    def Print(self):
        print("User Name:\t", self._name)
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

    def UnitTest(self, tst):
        tst.Message(f"User Unit Test user.py version 1.3.6.{User.Version - 1530}")

        self.LoginName = tst.TestValue("Login Name")
        self.Password = tst.TestValue("Password")
        self.Login()

        if tst.TestValue("Valid Login") is True:
            tst.Verify(self.IsLoggedIn, "User failed to log in")
        else:
            tst.Verify(self.IsLoggedIn == False, "User should not have logged in")

        tst.Verify(self.IsAdmin, "User is not an admin")
        
        self.Print()

        tst.Verify(self.LoginName == tst.TestValue("Login Name"), "Login name does not match expected value")
        tst.Verify(self.Password == tst.TestValue("Password"), "Password does not match expected value")


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

def test_user_test():
    tst = TestRunner()

    try:        
        if __version__  != User.Version:
            raise ImportError(f"Incompatible version: user.py is 1.3.6.{__version__ - 1530} but >= 1.3.6.{User.Version- 1530} is required.")

        usr = User()
        

        tst.TestValue("Login Name", "Vaughn")
        tst.TestValue("Password", "ZFyZH8DuKemv")
        tst.TestValue("Valid Login", True)
        usr.UnitTest(tst)
        usr.AddToMasterList("Nevin", "password")

        tst.Clear()
        tst.TestValue("Login Name", "guest")
        tst.TestValue("Password", "guest1")
        tst.TestValue("Valid Login", False)

        usr = User()        
        usr.UnitTest(tst)        

    except Exception as e:
        tst.Verify(False, f"Error running test: {e}")

    tst.Report()
    
    
    
    
if __name__ == "__main__":    
    test_user_test();
    


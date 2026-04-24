import MoeSzyslakPython
from MoeSzyslakPython import *

class User:
    
    _classID = 325850 

    def __init__(self):
        self._hObj = MoeSzyslakLibrary.CreateHandle(User._classID)
        print('User init')
        self.Name = ''
        self.Password = ''
        self._bIsLoggedIn = False

    def Dispose(self):
        if self._hObj > 0:
            MoeSzyslakLibrary.DestroyHandle(self._hObj)
            self._hOb = 0

    def Print(self):
        self.Update()
        print("User Name:\t", self.Name)
        print("Password:\t", self.Password)
        print("Is logged in:\t", self._bIsLoggedIn)

    def Commit(self):
        MoeSzyslakLibrary.Invoke(self._hObj, 'set "Login Name" "{0}"'.format(self.Name))
        MoeSzyslakLibrary.Invoke(self._hObj, 'set Password "{0}"'.format(self.Password))

    def Update(self):
        MoeSzyslakLibrary.Invoke(self._hObj, 'get "Login Name"')
        self.Name = MoeSzyslakLibrary.GetReturnString(self._hObj)    
        MoeSzyslakLibrary.Invoke(self._hObj, 'get Password')

        self.Password = MoeSzyslakLibrary.GetReturnString(self._hObj)
        MoeSzyslakLibrary.Invoke(self._hObj, 'get "Is Logged In"')

        if MoeSzyslakLibrary.GetReturnString(self._hObj) == "TRUE":
            self._bIsLoggedIn = True
        else:
            self._bIsLoggedIn = False


    def Login(self):
        self.Commit()
        MoeSzyslakLibrary.Invoke(self._hObj, "Login")
        self.Update()

    def UnitTest():
        usr = User()
        usr.Name = "Vaughn"
        usr.Password = "ZFyZH8DuKemv"
        usr.Login()

        usr.Print()
        usr.Dispose()

class Account:
    
    def __init__(self, hFinanceObj):
        self._hObj = hFinanceObj
        print('Account init')
        self.AccountName = "New Account"

    def Print(self):
        print("Account Name:\t", self.AccountName)

    def Commit(self):
        MoeSzyslakLibrary.Invoke(self._hObj, 'account set name "{0}"'.format(self.AccountName))

    def Update(self):
        MoeSzyslakLibrary.Invoke(self._hObj, 'account get name "{0}"'.format(self.AccountName))
        self.AccountName = MoeSzyslakLibrary.GetReturnString(self._hObj)

class Finance:
    
    _classID = 346141 

    def __init__(self):
        self._hObj = MoeSzyslakLibrary.CreateHandle(Finance._classID)
        print('FinancePage init')
        self.theAccount = Account(self._hObj)

    def Dispose(self):
        if self._hObj > 0:
            MoeSzyslakLibrary.DestroyHandle(self._hObj)
            self._hOb = 0

    def Print(self):
        self.Update()
        self.theAccount.Print()

    def Commit(self):
        self.theAccount.Commit()

    def Update(self):
        self.theAccount.Update()

    def UnitTest():
        fnc = Finance()
        fnc.theAccount.AccountName = "Wells Fargo"
        fnc.Commit()
        fnc.Print()

        fnc.Dispose()


if __name__ == "__main__":
    MoeSzyslakLibrary.VerifyLibrary()
    User.UnitTest()
        


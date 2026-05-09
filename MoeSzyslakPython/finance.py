import MoeSzyslakPython
from MoeSzyslakPython import *

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
        


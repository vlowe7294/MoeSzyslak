import ctypes
import os

from ctypes import *

class MoeSzyslakLibrary:

    _libPath = r"C:\Users\loweva\Visual Studio 2019\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll"
    _nMinVersion = 1468
    _hllDll = 0
    _sb = create_unicode_buffer(1000)
    _sbCapacity = 1000

    def VerifyLibrary():         
        if MoeSzyslakLibrary._hllDll == 0:
            MoeSzyslakLibrary._hllDll = ctypes.WinDLL(MoeSzyslakLibrary._libPath)
            hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32)
            MoeSzyslakLibrary.GetLibraryVersion = hllApiProto (("GetLibraryVersion", MoeSzyslakLibrary._hllDll))  
        
        v = MoeSzyslakLibrary.GetLibraryVersion()

        if v < MoeSzyslakLibrary._nMinVersion:
             print("Moe Szyslak Library Version 1.2.5." + str(v - 1275))
             raise Exception("Requires Moe Szyslak Library Version 1.2.5." + str(MoeSzyslakLibrary._nMinVersion)) 
        
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32)
        MoeSzyslakLibrary.CreateHandle = hllApiProto (("CreateMoeSzyslakHandle", MoeSzyslakLibrary._hllDll))
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32, ctypes.c_wchar_p)
        MoeSzyslakLibrary.InvokeHandle = hllApiProto (("InvokeMoeSzyslakHandle", MoeSzyslakLibrary._hllDll)) 

        hllApiProto = ctypes.WINFUNCTYPE (None, ctypes.c_uint32, ctypes.c_wchar_p, ctypes.c_uint32)
        MoeSzyslakLibrary.MoeSzyslakGetReturnString = hllApiProto (("MoeSzyslakGetReturnString", MoeSzyslakLibrary._hllDll)) 
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32)
        MoeSzyslakLibrary.DestroyHandle = hllApiProto (("DestroyMoeSzyslakHandle", MoeSzyslakLibrary._hllDll)) 

    def GetReturnString(hObj):
        ret = ''
        MoeSzyslakLibrary.MoeSzyslakGetReturnString(hObj, MoeSzyslakLibrary._sb, MoeSzyslakLibrary._sbCapacity)
        ret = MoeSzyslakLibrary._sb.value
        return ret;



class Neverwinter:

    _hObj = 0
    _classID = 977093

    def Start():
        MoeSzyslakLibrary.VerifyLibrary()
        Neverwinter._hObj = MoeSzyslakLibrary.CreateHandle(Neverwinter._classID)
        print(MoeSzyslakLibrary.GetReturnString(Neverwinter._hObj))
        MoeSzyslakLibrary.InvokeHandle(Neverwinter._hObj, "user Login");

    def Dispose():
        if Neverwinter._hObj > 0:
            MoeSzyslakLibrary.InvokeHandle(Neverwinter._hObj, "exit");
            print(MoeSzyslakLibrary.GetReturnString(Neverwinter._hObj))
            MoeSzyslakLibrary.DestroyHandle(Neverwinter._hObj)
            Neverwinter._hObj = 0

class IOS:

    _hObj = 0
    _classID = 306758

    def Start():
        MoeSzyslakLibrary.VerifyLibrary()
        IOS._hObj = MoeSzyslakLibrary.CreateHandle(IOS._classID)
        print(MoeSzyslakLibrary.GetReturnString(IOS._hObj))

    def Dispose():
        if IOS._hObj > 0:
            MoeSzyslakLibrary.DestroyHandle(IOS._hObj)
            IOS._hObj = 0




if __name__ == "__main__": 
    try:
        IOS.Start()
        IOS.Dispose()
    except Exception as e:
        print(e)
    

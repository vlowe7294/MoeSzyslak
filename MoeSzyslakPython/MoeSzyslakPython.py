import ctypes
import os
import pythoncom

from ctypes import POINTER, byref, c_int, c_void_p, HRESULT, WinError
from comtypes import IUnknown, GUID, COMMETHOD
from comtypes import CoInitialize, CoUninitialize, POINTER as CPOINTER
from comtypes.hresult import S_OK

from ctypes import *
from pathlib import Path

class MoeSzyslakLibrary:

    _libPath = r"C:\Users\Vaughn\Visual Studio\Sandbox\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll"
    #_libPath = str(Path(__file__).parent / "MoeSzyslakLibrary.dll")
    _nMinVersion = 1615
    _nVersion = 0
    _hllDll = 0
    _sb = create_unicode_buffer(1000)
    _sbCapacity = 1000

    def VerifyLibrary():         
        if MoeSzyslakLibrary._hllDll == 0:
            MoeSzyslakLibrary._hllDll = ctypes.WinDLL(MoeSzyslakLibrary._libPath)
            hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32)
            MoeSzyslakLibrary.GetLibraryVersion = hllApiProto (("GetLibraryVersion", MoeSzyslakLibrary._hllDll)) 
            
        else:
            return
        
        MoeSzyslakLibrary._nVersion = MoeSzyslakLibrary.GetLibraryVersion()

        if MoeSzyslakLibrary._nVersion < MoeSzyslakLibrary._nMinVersion:
             print(MoeSzyslakLibrary.Version())
             raise Exception("Requires Moe Szyslak Library Version 1.3.6." + str(MoeSzyslakLibrary._nMinVersion - 1530)) 
        
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32)
        MoeSzyslakLibrary.CreateHandle = hllApiProto (("CreateMoeSzyslakHandle", MoeSzyslakLibrary._hllDll))
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32, ctypes.c_wchar_p)
        MoeSzyslakLibrary.InvokeHandle = hllApiProto (("InvokeMoeSzyslakHandle", MoeSzyslakLibrary._hllDll)) 

        hllApiProto = ctypes.WINFUNCTYPE (None, ctypes.c_uint32, ctypes.c_wchar_p, ctypes.c_uint32)
        MoeSzyslakLibrary.MoeSzyslakGetReturnString = hllApiProto (("MoeSzyslakGetReturnString", MoeSzyslakLibrary._hllDll)) 
        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32)
        MoeSzyslakLibrary.DestroyHandle = hllApiProto (("DestroyMoeSzyslakHandle", MoeSzyslakLibrary._hllDll)) 

        hllApiProto = ctypes.WINFUNCTYPE (None, ctypes.c_uint32, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32)
        MoeSzyslakLibrary.MoeSzyslakHTML = hllApiProto (("MoeSzyslakHTML", MoeSzyslakLibrary._hllDll)) 

        MoeSzyslakLibrary.CreateMoeSzyslakInterface =  MoeSzyslakLibrary._hllDll.CreateMoeSzyslakInterface
        MoeSzyslakLibrary.CreateMoeSzyslakInterface.argtypes = [c_uint, ctypes.POINTER(c_void_p)]
        MoeSzyslakLibrary.CreateMoeSzyslakInterface.restype = None

    def GetReturnString(hObj, nCapacity = 1000):
        ret = ''

        if nCapacity > MoeSzyslakLibrary._sbCapacity:
                MoeSzyslakLibrary._sb = create_unicode_buffer(nCapacity)
                MoeSzyslakLibrary._sbCapacity = nCapacity

        MoeSzyslakLibrary.MoeSzyslakGetReturnString(hObj, MoeSzyslakLibrary._sb, MoeSzyslakLibrary._sbCapacity)
        ret = MoeSzyslakLibrary._sb.value
        return ret;

    def Version():
        return "Moe Szyslak Library Version 1.3.6." + str(MoeSzyslakLibrary._nVersion - 1530)

    def Invoke(hObj, strCmd):
        nRet = MoeSzyslakLibrary.InvokeHandle(hObj, strCmd)
        if nRet != 0:
             raise Exception("InvokeHandle returned failure code")

    def GetHTML(nClassID, strGet, nCapacity = 1000):
        ret = ''
        
        if nCapacity > MoeSzyslakLibrary._sbCapacity:
             MoeSzyslakLibrary._sb = create_unicode_buffer(nCapacity)
             MoeSzyslakLibrary._sbCapacity = nCapacity

        MoeSzyslakLibrary.MoeSzyslakHTML(nClassID, strGet, MoeSzyslakLibrary._sb, MoeSzyslakLibrary._sbCapacity)
        ret = MoeSzyslakLibrary._sb.value
        return ret

    def check_hresult(hr, message="COM call failed"):
        # HRESULT failure if high bit is set
        if hr & 0x80000000:
            raise RuntimeError(f"{message}: HRESULT=0x{hr:08X}")
        return hr




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
    

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
    _nMinVersion = 1643
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

        hllApiProto = ctypes.WINFUNCTYPE (ctypes.c_uint32, ctypes.c_uint32)
        MoeSzyslakLibrary.DestroyHandle = hllApiProto (("DestroyMoeSzyslakHandle", MoeSzyslakLibrary._hllDll)) 

        hllApiProto = ctypes.WINFUNCTYPE (None, ctypes.c_uint32, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32)
        MoeSzyslakLibrary.MoeSzyslakHTML = hllApiProto (("MoeSzyslakHTML", MoeSzyslakLibrary._hllDll)) 

        MoeSzyslakLibrary.CreateMoeSzyslakInterface =  MoeSzyslakLibrary._hllDll.CreateMoeSzyslakInterface
        MoeSzyslakLibrary.CreateMoeSzyslakInterface.argtypes = [c_uint, ctypes.POINTER(c_void_p)]
        MoeSzyslakLibrary.CreateMoeSzyslakInterface.restype = None

        MoeSzyslakLibrary.DispatchMoeSzyslak =  MoeSzyslakLibrary._hllDll.DispatchMoeSzyslak
        MoeSzyslakLibrary.DispatchMoeSzyslak.argtypes = [ctypes.c_uint, ctypes.c_int, ctypes.c_void_p]
        MoeSzyslakLibrary.DispatchMoeSzyslak.restype  = ctypes.c_int

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


class MoeVariant(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_int),
        ("data", ctypes.c_void_p),
        ("sizeBytes", ctypes.c_size_t)
    ] 

class MoeVariantList(ctypes.Structure):
    _fields_ = [
        ("count", ctypes.c_size_t),
        ("items", ctypes.c_void_p)   # pointer to array of MoeVariant
    ]

class MoeDispatch():
    def make_variant_array(variants):
        count = len(variants)
        array_type = MoeVariant * count
        array_obj = array_type(*variants)

        ptr = ctypes.cast(array_obj, ctypes.c_void_p)
        return ptr, array_obj  # return both pointer and Python-owned array

    def make_variant_list(variants):
        items_ptr, array_obj = MoeDispatch.make_variant_array(variants)

        list_obj = MoeVariantList()
        list_obj.count = len(variants)
        list_obj.items = items_ptr

        list_ptr = ctypes.cast(ctypes.pointer(list_obj), ctypes.c_void_p)

        return list_ptr, list_obj, array_obj

    def set_property(handle, name, value):
        # Allocate strings
        name_buf  = ctypes.create_unicode_buffer(name)
        value_buf = ctypes.create_unicode_buffer(value)

        # Build variants
        v_name = MoeVariant(
            type=0,
            data=ctypes.cast(name_buf, ctypes.c_void_p),
            sizeBytes=(len(name) + 1) * 2
        )

        v_value = MoeVariant(
            type=0,
            data=ctypes.cast(value_buf, ctypes.c_void_p),
            sizeBytes=(len(value) + 1) * 2
        )

        # Build variant list
        list_ptr, list_obj, array_obj = MoeDispatch.make_variant_list([v_name, v_value])

        # Dispatch
        result = MoeSzyslakLibrary.DispatchMoeSzyslak(handle, 1, list_ptr)
        return result




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
    MoeSzyslakLibrary.VerifyLibrary()
    handle = MoeSzyslakLibrary.CreateHandle(507734)

    value = "Summer Vacation"
    MoeDispatch.set_property(handle, "TripName", "Summer Vacation")
    print("TripName =", value)
    

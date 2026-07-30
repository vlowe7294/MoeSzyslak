using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Xml.Linq;

namespace MoeConsole
{
    public static class MoeSzyslakLibrary
    {
        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern uint GetLibraryVersion();

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern uint CreateMoeSzyslakHandle(uint nClassID);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern uint InvokeMoeSzyslakHandle(uint hObj, uint hVariables);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern uint DestroyMoeSzyslakHandle(uint hObj);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern void MoeSzyslakGetReturnString(uint hObj, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder szRet, uint len);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern void MoeSzyslakHTML(uint nClassID, [MarshalAs(UnmanagedType.LPWStr)] string strGet, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder szRet, uint len);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern void CreateMoeSzyslakInterface(uint nClassID, ref IntPtr iunk);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern void FreeMoeSzyslakInterface(IntPtr iunk);

        public enum CLASSID
        {
            INVALIDCLASS = 0,            
            TESTING = 398981,            
            TRIPPLANNER = 507734,
            DATABASE = 781903,
            NEVERWINTER = 977093
        }

        public static void VerifyLibrary()
        {
            uint v = MoeSzyslakLibrary.GetLibraryVersion();

            string result = string.Format("Current Moe Szyslak Library {0}", MoeSzyslakLibrary.Version);
            result += string.Format("\nRequires Moe Szyslak Library Version 1.3.6.{0}", m_nMinVersion - 1530);

            if (v != m_nMinVersion)
                throw new Exception(result);
        }

        public static uint CreateHandle(CLASSID nClassID)
        {
            return MoeSzyslakLibrary.CreateMoeSzyslakHandle((uint)nClassID);
        }

        static public string Version
        {
            get
            {
                uint v = MoeSzyslakLibrary.GetLibraryVersion();
                return string.Format("Version 1.3.6.{0}", v - 1530);
            }
        }

        static public string GetHTML(CLASSID nClassID, uint nBufLen, string strGet)
        {
            if (nBufLen > m_sb.Capacity)
                m_sb = new StringBuilder((int)nBufLen);

            MoeSzyslakHTML((uint)nClassID, strGet, m_sb, (uint)m_sb.Capacity);
            return m_sb.ToString();

        }

        public const string LibraryPath = @"C:\Users\Vaughn\Visual Studio\Sandbox\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll";
        static private uint m_nMinVersion = 1657;
        static private StringBuilder m_sb = new StringBuilder(255);
    }

    static class MoeDispatch
    {
        [DllImport(MoeSzyslakLibrary.LibraryPath)]
        public static extern int DispatchMoeSzyslak(uint hObj, int methodId, IntPtr args);

        public enum MoeVariantType : int
        {
            MV_EMPTY = 0,
            MV_INT = 1,
            MV_DOUBLE = 2,
            MV_STRING = 3,
            MV_BOOL = 4,
            MV_PTR = 5
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct MOEVARIANT
        {
            public MoeVariantType type;
            public IntPtr data;
            public UIntPtr sizeBytes;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MOEVARIANTLIST
        {
            public UIntPtr count;
            public IntPtr items; // pointer to array of MoeVariant
        }

        static void SetVariant(int ndx, string val)
        {
            Marshal.FreeHGlobal(m_vars[ndx].data);
            m_vars[ndx].type = MoeVariantType.MV_STRING;
            m_vars[ndx].data = Marshal.StringToHGlobalUni(val);

            int nOffset = ndx * Marshal.SizeOf<MOEVARIANT>();

            m_vars[ndx].sizeBytes = (UIntPtr)((val.Length + 1) * 2);
            Marshal.StructureToPtr(m_vars[ndx], m_itemsPtr + nOffset, false);            
        }        

        static public void Dispatch(uint hDisp)
        {
            SetVariant(0, "TripName");
            SetVariant(1, "Summer Vacation");
                        
            // Marshal list
            m_list.count = (UIntPtr)2;
            m_list.items = m_itemsPtr;

            Marshal.StructureToPtr(m_list, m_listPtr, false);
            DispatchMoeSzyslak(hDisp, 1, m_listPtr);        
        }

        static MOEVARIANT[] m_vars = new MOEVARIANT[2];
        static IntPtr m_itemsPtr = Marshal.AllocHGlobal(Marshal.SizeOf<MOEVARIANT>() * 2);
        static MOEVARIANTLIST m_list = new MOEVARIANTLIST();
        static IntPtr m_listPtr = Marshal.AllocHGlobal(Marshal.SizeOf<MOEVARIANTLIST>());

    }



}
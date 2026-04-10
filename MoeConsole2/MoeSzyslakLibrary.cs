using System;
using System.Runtime.InteropServices;
using System.Text;

namespace MoeConsole
{
    public static class MoeSzyslakLibrary
    {
        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern uint GetLibraryVersion();

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern uint CreateMoeSzyslakHandle(uint nClassID);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern uint InvokeMoeSzyslakHandle(uint hObj, [MarshalAs(UnmanagedType.LPWStr)] string szCmd);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        public static extern uint DestroyMoeSzyslakHandle(uint hObj);

        [DllImport(LibraryPath, CharSet = CharSet.Unicode)]
        private static extern void MoeSzyslakGetReturnString(uint hObj, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder szRet, uint len);

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

        public static void InvokeHandle(uint hObj, string strCmd)
        {
            uint rslt = InvokeMoeSzyslakHandle(hObj, strCmd);

            if (rslt != 0)
                throw new Exception("InvokeMoeSzyslakHandle returned error");

        }

        public static string GetReturnString(uint hObj, uint nBufLen = 0)
        {
            if (nBufLen > m_sb.Capacity)
                m_sb = new StringBuilder((int)nBufLen);

            MoeSzyslakGetReturnString(hObj, m_sb, (uint)m_sb.Capacity);
            return m_sb.ToString();
        }

        static public string Version
        {
            get
            {
                uint v = MoeSzyslakLibrary.GetLibraryVersion();
                return string.Format("Version 1.3.6.{0}", v - 1530);
            }
        }

        public const string LibraryPath = @"C:\Users\Vaughn\Visual Studio\Sandbox\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll";
        static private uint m_nMinVersion = 1559;
        static private StringBuilder m_sb = new StringBuilder(255);
    }



}
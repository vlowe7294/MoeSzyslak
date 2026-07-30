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
            result += string.Format("\nRequires Moe Szyslak Library Version 1.2.5.{0}", m_nMinVersion - 1275);

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

        static public string Version
        {
            get
            {
                uint v = MoeSzyslakLibrary.GetLibraryVersion();
                return string.Format("Version 1.2.5.{0}", v - 1275);
            }
        }

        public const string LibraryPath = @"C:\Users\loweva\Visual Studio 2019\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll";
        static private uint m_nMinVersion = 1524;
        static private StringBuilder m_sb = new StringBuilder(255);
    }



}
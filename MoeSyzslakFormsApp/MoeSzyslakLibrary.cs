using System;
using System.Runtime.InteropServices;
using System.Text;

namespace MoeSyzslakFormsApp
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
            NEVERWINTER = 977093
        }

        static public string Version
        {
            get
            {
                try
                {
                    uint v = MoeSzyslakLibrary.GetLibraryVersion();
                    return string.Format("Current Moe Szyslak Library Version 1.2.5.{0}", v - 1275);

                }
                catch (DllNotFoundException exc)
                {
                    return "Moe Szyslak Library not available";
                }
            }
        }

        static public void VerifyLibrary()
        {
            if (!System.IO.File.Exists(LibraryPath))
                throw new Exception(string.Format("File {0} does not exist", LibraryPath));

            if (MoeSzyslakLibrary.GetLibraryVersion() != m_nMinVersion)
                throw new Exception(string.Format("Requires Moe Szyslak Library Version 1.2.5.{0}", m_nMinVersion - 1275));
        }

        public static uint CreateHandle(CLASSID nClassID)
        {
            try
            {
                return MoeSzyslakLibrary.CreateMoeSzyslakHandle((uint)nClassID);
            }
            catch (DllNotFoundException exc)
            {
                return 0;
            }
        }

        public static bool InvokeHandle(uint hObj, string strCmd)
        {
            try
            {
                uint rslt = InvokeMoeSzyslakHandle(hObj, strCmd);

                if (rslt != 0)
                {
                    LastError = "InvokeHandle returned error code";
                    return false;
                }
                else
                    return true;

            }
            catch (DllNotFoundException exc)
            {
                LastError = exc.Message;
                return false;
            }
        }

        public static string GetReturnString(uint hObj, uint nBufLen = 0)
        {
            try
            {
                if (nBufLen > m_sb.Capacity)
                    m_sb = new StringBuilder((int)nBufLen);

                MoeSzyslakGetReturnString(hObj, m_sb, (uint)m_sb.Capacity);
            }
            catch (Exception exc)
            {
                return "";
            }

            return m_sb.ToString();
        }

        public const string LibraryPath = @"C:\Users\loweva\Visual Studio 2019\MoeSzyslak\x64\Debug\MoeSzyslakLibrary.dll";
        static private StringBuilder m_sb = new StringBuilder(255);
        const uint m_nMinVersion = 1511;
        public static string LastError = "";
    }
}
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace MoeConsole
{
    public class LogEntry
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("AC597284-A906-482F-9BE9-C4E13AB43E50")]
        private interface ILOGENTRY
        {
            void GetText([MarshalAs(UnmanagedType.BStr)] out string strTxt);
            void SetText([MarshalAs(UnmanagedType.LPWStr)] string strTxt);
            void GetTime(ref UInt64 tme);
            void GetMemUsed(ref UInt64 mem);
            void SetDebugLevel(int nDebug);
	        void GetDebugLevel(ref int nDebug);
        }

        public LogEntry(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iLogEntry = (ILOGENTRY)Marshal.GetObjectForIUnknown(m_iunk);
            m_iLogEntry.GetText(out m_strText);
            m_iLogEntry.GetTime(ref m_nTime);
            m_iLogEntry.GetMemUsed(ref m_memoryUsed);
            m_iLogEntry.GetDebugLevel(ref m_nDebugLevel);
        }

        public void Dispose()
        {
            if (m_iLogEntry != null)
            {
                Marshal.ReleaseComObject(m_iLogEntry);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
                m_iLogEntry = null;
            }
        }

        public void Print()
        {
           Console.WriteLine(string.Format("\t{0}\t{1}\t{2}", m_nTime, m_strText, m_memoryUsed));
        }

        private IntPtr m_iunk;
        private ILOGENTRY m_iLogEntry;

        public string Text
        {
            get { return m_strText; }
            set
            {
                m_strText = value;
                m_iLogEntry.SetText(m_strText);
            }
        }
        private string m_strText;

        public int DebugLevel
        {
            get { return m_nDebugLevel; }
            set
            {
                m_nDebugLevel = value;
                m_iLogEntry.SetDebugLevel(m_nDebugLevel);
            }
        }
        private int m_nDebugLevel = 0;

        private UInt64 m_nTime;
        private UInt64 m_memoryUsed;
    }

    public class Testing 
    {
        static public uint classID = 398981;

        public enum DEBUG_LEVEL
        {
            DEBUG_FULL,
            DEBUG_VERBOSE,
            DEBUG_INFO,
            DEBUG_WARN,
            DEBUG_CRITICAL
        };

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("7C6DA0F8-84AE-4E97-86F0-13BB1E67C713")]
        private interface ITESTING
        {
            void Message([MarshalAs(UnmanagedType.LPWStr)] string szMsg, int nDebugLvl, [MarshalAs(UnmanagedType.LPWStr)] string strCategory);
            void SetDebugLevel(int level);
            void GetLogEntry(int ndx, ref IntPtr iEntry);
            void VerifyVariable([MarshalAs(UnmanagedType.LPWStr)] string varName, [MarshalAs(UnmanagedType.LPWStr)] string val);
		    void VerifyHResult(uint hr, [MarshalAs(UnmanagedType.LPWStr)] string szMsg);
		    void GetTestData([MarshalAs(UnmanagedType.LPWStr)] string szName, [MarshalAs(UnmanagedType.BStr)] out string iStr);
		    void SetTestData([MarshalAs(UnmanagedType.LPWStr)] string szName, [MarshalAs(UnmanagedType.LPWStr)] string szVal);
		    void GetClassName(uint nClassID, IntPtr iStrClassName);
		    void GetClassID([MarshalAs(UnmanagedType.LPWStr)] string szClassName, ref uint nClassID);
		    void Report([MarshalAs(UnmanagedType.LPWStr)] StringBuilder szRpt, uint nlen);
		    void Verify(uint bVal, [MarshalAs(UnmanagedType.LPWStr)] string szMsg, int nDebugLvl, [MarshalAs(UnmanagedType.LPWStr)] string szCategory);
            void GetPassed(ref uint bVal);
            void NewEntry(ref IntPtr iEntry);
            void UnitTest();
        }

        public Testing(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iTesting = (ITESTING)Marshal.GetObjectForIUnknown(m_iunk);

        }

        ~Testing()
        {
            Dispose();
        }

        public void Dispose()
        {
            foreach (LogEntry le in m_logEntries)
                le.Dispose();

            m_logEntries.Clear();

            if (m_iTesting != null)
            {
                Marshal.ReleaseComObject(m_iTesting);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iTesting = null;
                m_iunk = IntPtr.Zero;
            }
        }

        public void Message(string strMsg, DEBUG_LEVEL dbg, string strCategory)
        {
            m_iTesting.Message(strMsg, (int)dbg, strCategory);
        }

        public void Report()
        {
            Update();
            Console.WriteLine("\tStart time(ms)\tText\tMemory Used");
            int n = 0;


            foreach (LogEntry le in m_logEntries)
            {
                Console.Write(n.ToString());
                le.Print();
                n++;
            }

            uint r = 0;
            m_iTesting.GetPassed(ref r);

            if (r > 0)
                Console.WriteLine("Result:  PASS");
            else
                Console.WriteLine("Result:  FAIL");

        }

        public void Verify(bool bVal, string szMsg, int nDebugLevel, string strCategory)
        {
            uint n = 0;

            if (bVal)
                n = 1;

            m_iTesting.Verify(n, szMsg, nDebugLevel, strCategory);

        }


        private IntPtr m_iunk;
        private ITESTING m_iTesting;
        static private StringBuilder m_strBuilder = new StringBuilder(1024);

        public void RunTest(uint nClassID)
        {
            if (m_bMemoryCheck)
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" TRUE");
            else
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" FALSE");

            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("RunTest {0}", nClassID));

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "log get count");

            string cnt = MoeSzyslakLibrary.GetReturnString(m_hObj);
            m_logEntries.Clear();

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Passed");
            m_bPassed = MoeSzyslakLibrary.GetReturnString(m_hObj) == "TRUE";


            if (m_bPassed)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Result:  PASS");
                Console.ForegroundColor = ConsoleColor.White;
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Result:  FAILED");
                Console.ForegroundColor = ConsoleColor.White;
            }
        }

        

        public void Load(string flePath)
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "load \"" + flePath + "\"");
        }

        public void Update()
        {
            int nCnt = 0;
            IntPtr iunk = IntPtr.Zero;

            foreach (LogEntry le in m_logEntries)
                le.Dispose();

            m_logEntries.Clear();

            m_iTesting.GetLogEntry(nCnt, ref iunk);

            while (iunk != IntPtr.Zero)
            {
                m_logEntries.Add(new LogEntry(iunk));
                iunk = IntPtr.Zero;
                nCnt++;
                m_iTesting.GetLogEntry(nCnt, ref iunk);
            }

        }

        public LogEntry GetLogEntry(int ndx)
        {
            return m_logEntries[ndx];
        }
        public LogEntry NewEntry()
        {
            IntPtr iEntry = IntPtr.Zero;
            m_iTesting.NewEntry(ref iEntry);
            LogEntry l = new LogEntry(iEntry);
            m_logEntries.Add(l);
            return l;
        }

        public void SetTestData(string strName, string strVal)
        {
            m_iTesting.SetTestData(strName, strVal);
        }

        public string GetTestData(string strName)
        {
            string strVal;
            m_iTesting.GetTestData(strName, out strVal);
            return strVal;
        }

        public void VerifyVariable(string varName, string val)
        {
            m_iTesting.VerifyVariable(varName, val);
        }
        static public void UnitTest()
        {
            using (CampSight cs = new CampSight())
            {
                Testing tst = cs.theTester;
                try
                {

                    tst.DebugLevel = DEBUG_LEVEL.DEBUG_VERBOSE;
                    tst.SetTestData("message", "Log Entry unit test");

                    tst.Message("Testing Object Self Unit Test", Testing.DEBUG_LEVEL.DEBUG_INFO, "Testing");                    
                    tst.Update();
                    LogEntry l = tst.NewEntry();

                    l.Text = tst.GetTestData("message");
                    tst.VerifyVariable("message", l.Text);
                    l.DebugLevel = (int)DEBUG_LEVEL.DEBUG_INFO;

                    //tst.m_iTesting.UnitTest();

                }
                catch (Exception ex)
                {
                    tst.Verify(false, ex.Message, (int)Testing.DEBUG_LEVEL.DEBUG_CRITICAL, "Testing");
                }

                tst.Report();
            }
        }

        private uint m_hObj;
        private Dictionary<string, string> m_testValues = new Dictionary<string, string>();
        private bool m_bPassed = true;

        private List<LogEntry> m_logEntries = new List<LogEntry>();


        public bool MemoryCheck
        {
            set { m_bMemoryCheck = value; }
            get { return m_bMemoryCheck; }
        }
        private bool m_bMemoryCheck = false;

        public DEBUG_LEVEL DebugLevel
        {
            set 
            {
                m_iTesting.SetDebugLevel((int)value); 
            }

        }


    }

}
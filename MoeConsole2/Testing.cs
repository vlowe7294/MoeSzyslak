using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace MoeConsole
{
    public class LogEntry
    {
        public LogEntry(IntPtr iunk)
        {
            m_iunk = iunk;
        }

        public void Dispose()
        {
            if (m_iunk != IntPtr.Zero)
            {
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
            }
        }

        public void Print()
        {
            if (m_ndx == 0)
                Console.WriteLine("Time\tText\tMemory");

            Console.WriteLine(string.Format("{0}\t{1}\t{2}", m_nTime, m_strText, m_memoryUsed));
        }

        private IntPtr m_iunk;
        private uint m_ndx;
        private string m_strText;
        private int m_nTime;
        private int m_memoryUsed;

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
            void Message([MarshalAs(UnmanagedType.LPWStr)] string szMsg, int nDebugLvl);
            void SetDebugLevel(int level);
            void GetLogEntry(int ndx, ref IntPtr iEntry);
            void RunTest(uint nClassID);
		    void VerifyVariable([MarshalAs(UnmanagedType.LPWStr)] string varName, [MarshalAs(UnmanagedType.LPWStr)] string val);
		    void VerifyHResult(uint hr, [MarshalAs(UnmanagedType.LPWStr)] string szMsg);
		    void GetTestData([MarshalAs(UnmanagedType.LPWStr)] string szName, IntPtr iStr);
		    void SetTestData([MarshalAs(UnmanagedType.LPWStr)] string szName, [MarshalAs(UnmanagedType.LPWStr)] string szVal);
		    void GetClassName(uint nClassID, IntPtr iStrClassName);
		    void GetClassID([MarshalAs(UnmanagedType.LPWStr)] string szClassName, ref uint nClassID);
		    void Report([MarshalAs(UnmanagedType.LPWStr)] StringBuilder szRpt, uint nlen);
		    void Verify(uint bVal, [MarshalAs(UnmanagedType.LPWStr)] string szMsg);
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

        public void Message(string strMsg, DEBUG_LEVEL dbg)
        {
            m_iTesting.Message(strMsg, (int)dbg);
        }

        public string Report()
        {
            m_iTesting.Report(m_strBuilder, (uint)m_strBuilder.Capacity);
            return m_strBuilder.ToString();

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

        

        public void SetTestValue(string name, string val)
        {
            m_testValues[name] = val;
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("SetTestData \"{0}\" \"{1}\"", name, val));
        }

        public string GetTestValue(string name)
        {
            
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("TestValues get \"{0}\"", name));
            m_testValues[name] = MoeSzyslakLibrary.GetReturnString(m_hObj);
            return m_testValues[name];
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

        static public void UnitTest()
        {
            try
            {
                using (CampSight cs = new CampSight())
                {
                    Testing tst = cs.theTester;                    

                    tst.DebugLevel = DEBUG_LEVEL.DEBUG_CRITICAL;
                    tst.Message("Testing Object Self Unit Test", Testing.DEBUG_LEVEL.DEBUG_INFO);
                    tst.Update();

                    

                    // tst.m_iTesting.UnitTest();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Unit Test Failed: " + ex.Message);
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

        DEBUG_LEVEL DebugLevel
        {
            set 
            {
                m_iTesting.SetDebugLevel((int)value); 
            }

        }


    }

}
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.CompilerServices;

namespace MoeConsole
{
    public class VariableCollection
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("8AFF6079-A5CA-4CF5-935E-8D1E9C954F83")]
        private interface IVARIABLELIST
        {
            void GetAsString([MarshalAs(UnmanagedType.LPWStr)] string szTag, [MarshalAs(UnmanagedType.BStr)] out string bsStr);
        }

        public VariableCollection(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iVariableList = (IVARIABLELIST)Marshal.GetObjectForIUnknown(m_iunk);
        }

        public void Dispose()
        {
            if (m_iVariableList != null)
            {
                Marshal.ReleaseComObject(m_iVariableList);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
                m_iVariableList = null;
            }
        }

        public string GetAsString(string szTag)
        {
            string strVal;
            m_iVariableList.GetAsString(szTag, out strVal);
            return strVal;
        }

        IntPtr m_iunk;
        IVARIABLELIST m_iVariableList;

    }

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
            void GetCategory([MarshalAs(UnmanagedType.BStr)] out string strCat);
	        void SetCategory([MarshalAs(UnmanagedType.LPWStr)] string strCat);
            void GetFile([MarshalAs(UnmanagedType.BStr)] out string bsFile);
	        void SetFile([MarshalAs(UnmanagedType.LPWStr)] string szFile);
            void GetDeltaTime(ref Int64 nDeltaTime);
            void GetLine(ref int nLine);
	        void SetLine(int nLine);
        }

        public LogEntry(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iLogEntry = (ILOGENTRY)Marshal.GetObjectForIUnknown(m_iunk);
            m_iLogEntry.GetText(out m_strText);
            m_iLogEntry.GetTime(ref m_nTime);
            m_iLogEntry.GetMemUsed(ref m_memoryUsed);
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
        private string m_strText = "";

        public int DebugLevel
        {
            get 
            {
                m_iLogEntry.GetDebugLevel(ref m_nDebugLevel); 
                return m_nDebugLevel; 
            }
            set
            {
                m_nDebugLevel = value;
                m_iLogEntry.SetDebugLevel(m_nDebugLevel);
            }
        }
        private int m_nDebugLevel = 0;

        public string Category
        {
            get
            {
                m_iLogEntry.GetCategory(out m_strCategory);
                return m_strCategory;
            }
            set
            {
                m_strCategory = value;
                m_iLogEntry.SetCategory(m_strCategory);
            }
        }
        private string m_strCategory = "";

        public string File
        {
            get
            {
                m_iLogEntry.GetFile(out m_strFile);
                return m_strFile;
            }
            set
            {
                m_strFile = value;
                m_iLogEntry.SetFile(m_strFile);
            }
        }
        private string m_strFile = "";

        public Int64 DeltaTime
        {
            get
            {
                m_iLogEntry.GetDeltaTime(ref m_deltaTime);
                return m_deltaTime;            }
            
        }
        private Int64 m_deltaTime = 0;

        public int Line
        {
            get
            {
                m_iLogEntry.GetLine(ref m_nLine);
                return m_nLine;
            }
            set
            {
                m_nLine = value;
                m_iLogEntry.SetLine(m_nLine);
            }
        }
        private int m_nLine = 0;

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
		    void GetClassName(uint nClassID, [MarshalAs(UnmanagedType.BStr)] out string strClassName);
		    void GetClassID([MarshalAs(UnmanagedType.LPWStr)] string szClassName, ref uint nClassID);
		    void Report();
		    void Verify(uint bVal, [MarshalAs(UnmanagedType.LPWStr)] string szMsg, int nDebugLvl, [MarshalAs(UnmanagedType.LPWStr)] string szCategory);
            void GetPassed(ref uint bVal);
            void NewEntry(ref IntPtr iEntry);
            void GetProperties(ref IntPtr iPrp);
            void UnitTest();
        }

        public Testing(IntPtr iunk)
        {
            IntPtr iPrp = IntPtr.Zero;

            m_iunk = iunk;
            m_iTesting = (ITESTING)Marshal.GetObjectForIUnknown(m_iunk);
            m_iTesting.GetProperties(ref iPrp);

            m_properties = new VariableCollection(iPrp);

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

            m_properties.Dispose();
        }

        public void Message(string strMsg, DEBUG_LEVEL dbg, string strCategory)
        {
            m_iTesting.Message(strMsg, (int)dbg, strCategory);
        }

        public void Report()
        {
            m_iTesting.Report();
            Console.WriteLine(m_properties.GetAsString("Report"));
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
            

            foreach (LogEntry le in m_logEntries)
                le.Dispose();

            m_logEntries.Clear();
            LogEntry newEntry = GetLogEntry(0);

            while (newEntry != null)
            {
                m_logEntries.Add(newEntry);
                nCnt++;
                newEntry = GetLogEntry(nCnt);
            }
        }

        private LogEntry GetLogEntry(int ndx)
        {
            IntPtr iunk = IntPtr.Zero;
            m_iTesting.GetLogEntry(ndx, ref iunk);

            if (iunk != IntPtr.Zero)
                return new LogEntry(iunk);
            else
                return null;
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

        public string GetClassName(uint nClassID)
        {
            string strClassName;
            m_iTesting.GetClassName(nClassID, out strClassName);
            return strClassName;
        }

        public uint GetClassID(string strClassName)
        {
            uint nClassID = 0;
            m_iTesting.GetClassID(strClassName, ref nClassID);
            return nClassID;
        }

        VariableCollection m_properties;

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
                    l.File = "Testing.cs";
                    l.Line = 355;                    

                    Console.WriteLine("Log Entry Delta Time: {0}", l.DeltaTime);

                    string cls = tst.GetClassName(Testing.classID);
                    uint clsID = tst.GetClassID(cls);
                    tst.Verify(clsID == Testing.classID, "Class ID mismatch", (int)DEBUG_LEVEL.DEBUG_CRITICAL, "Testing");
                    tst.Verify(l.File == "Testing.cs", "File name mismatch", (int)DEBUG_LEVEL.DEBUG_CRITICAL, "Testing");
                    l.Category = cls;                   


                    tst.m_iTesting.UnitTest();

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


        public List<LogEntry> LogEntries
        {
            get { return m_logEntries; }
        }
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
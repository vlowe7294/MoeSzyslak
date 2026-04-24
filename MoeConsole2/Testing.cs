namespace MoeConsole
{
    public class LogEntry
    {
        public LogEntry(uint hTesting, uint ndx)
        {
            m_hTesting = hTesting;
            m_ndx = ndx;
            MoeSzyslakLibrary.InvokeHandle(m_hTesting, string.Format("log {0} get text", m_ndx));
            m_strText = MoeSzyslakLibrary.GetReturnString(m_hTesting);

            MoeSzyslakLibrary.InvokeHandle(m_hTesting, string.Format("log {0} get time", m_ndx));
            m_nTime = int.Parse(MoeSzyslakLibrary.GetReturnString(m_hTesting));

            MoeSzyslakLibrary.InvokeHandle(m_hTesting, string.Format("log {0} get \"memory used\"", m_ndx));
            m_memoryUsed = int.Parse(MoeSzyslakLibrary.GetReturnString(m_hTesting));
        }

        public void Print()
        {
            if (m_ndx == 0)
                Console.WriteLine("Time\tText\tMemory");

            Console.WriteLine(string.Format("{0}\t{1}\t{2}", m_nTime, m_strText, m_memoryUsed));
        }

        private uint m_hTesting;
        private uint m_ndx;
        private string m_strText;
        private int m_nTime;
        private int m_memoryUsed;

    }

    public class Testing
    {
        public Testing()
        {
            m_hObj = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.TESTING);

        }

        ~Testing()
        {
            Dispose();
        }

        public void Dispose()
        {

            if (m_hObj > 0)
            {
                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hObj);
                m_hObj = 0;
            }
        }

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

            for (uint i = 0; i < uint.Parse(cnt); i++)
            {
                m_logEntries.Add(new LogEntry(m_hObj, i));
                m_logEntries[(int)i].Print();
            }

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

        public void Message(string strMsg)
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("Message \"{0}\"", strMsg));
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

        static public void UnitTest()
        {
            string message = "Testing Object Self Unit Test";

            try
            {
                Database db = new Database();
                Testing tst = new Testing();

                tst.SetTestValue("message", message);
                //tst.Load(@"C:\Users\loweva\Visual Studio 2019\MoeSzyslak\MoeConsole\bin\Debug\net5.0\testing.db");

                tst.Message("Testing Object Self Unit Test");
                tst.RunTest((uint)MoeSzyslakLibrary.CLASSID.TESTING);

                if (tst.GetTestValue("message") != message)
                    throw new Exception("message did not match expected value.");

                if (tst.m_logEntries.Count != 5)
                    throw new Exception("Should be 5 log entries.");


                tst.Dispose();
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


    }

}
namespace MoeConsole
{
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
            foreach (KeyValuePair<string, string> kvp in m_testValues)
            {
                MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("SetTestData \"{0}\" \"{1}\"", kvp.Key, kvp.Value));
            }

            if (m_bMemoryCheck)
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" TRUE");
            else
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" FALSE");

            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("RunTest {0}", nClassID));
            Console.WriteLine(MoeSzyslakLibrary.GetReturnString(m_hObj, 500));

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
        }

        public void Load(string flePath)
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "load \"" + flePath + "\"");
        }

        static public void UnitTest()
        {
            Database db = new Database();
            Testing tst = new Testing();

            tst.SetTestValue("message", "Testing Object Self Unit Test");
            //tst.Load(@"C:\Users\loweva\Visual Studio 2019\MoeSzyslak\MoeConsole\bin\Debug\net5.0\testing.db");

            tst.RunTest((uint)MoeSzyslakLibrary.CLASSID.TESTING);
            db.Dispose();
            tst.Dispose();
        }

        private uint m_hObj;
        private Dictionary<string, string> m_testValues = new Dictionary<string, string>();
        private bool m_bPassed = true;


        public bool MemoryCheck
        {
            set { m_bMemoryCheck = value; }
            get { return m_bMemoryCheck; }
        }
        private bool m_bMemoryCheck = false;


    }

    public class Database
    {
        public Database()
        {
            m_hObj = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.DATABASE);
        }

        ~Database()
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

        private uint m_hObj;
    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Console.WriteLine(MoeSzyslakLibrary.Version);
                TripPlanner.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Collections.Generic;

namespace MoeSzyslakTest
{
    [TestClass]
    public class LibraryTest
    {
        [TestMethod]
        public void VerifyLibrary()
        {
            MoeSzyslakLibrary.VerifyLibrary();
        }

        [TestMethod]
        public void TripPlannerTest()
        {
            TripPlanner trp = new TripPlanner();
            Testing tst = new Testing();
            Database db = new Database();

            tst.RunTest((uint)MoeSzyslakLibrary.CLASSID.TRIPPLANNER);

            tst.RunTest(VLVariable.ClassID);

            trp.Load(@"C:\Users\loweva\Visual Studio 2019\JimboJones\JimboJonesTest\trip.dat");
            tst.Dispose();
            db.Dispose();
        }

        [TestMethod]
        public void IOSTest()
        {
            IOS ios = new IOS();
        }

        [TestMethod]
        public void NeverwinterTest()
        {
            NeverwinterNights nwn = new NeverwinterNights();
        }

        [TestMethod]
        public void FinanceTest()
        {
            Testing tst = new Testing();
            const uint classID = 346141;

            tst.SetTestValue("count", "500");
            tst.SetTestValue("object type", "87153");
            tst.SetTestValue("command", "206 set value XORN");

            tst.RunTest(classID);
            tst.Dispose();

            tst.Dispose();
        }
    }

    public class TripPlanner
    {
        public TripPlanner()
        {
            m_hTripPlanner = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.TRIPPLANNER);
        }

        public void Load(string strPath)
        {
            MoeSzyslakLibrary.InvokeHandle(m_hTripPlanner, string.Format("Load \"{0}\"", strPath));

        }

        uint m_hTripPlanner = 0;

    }

    public class IOS
    {
        public IOS()
        {

        }

    }

    public class Module
    {

        public Module(uint hNeverwinter)
        {


        }
    }

    public class NeverwinterNights
    {
        public NeverwinterNights()
        {
            m_hNeverwinterNights = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.NEVERWINTER);
            m_module = new Module(m_hNeverwinterNights);

        }

        ~NeverwinterNights()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (m_hNeverwinterNights > 0)
            {
                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hNeverwinterNights);
                m_hNeverwinterNights = 0;
            }

        }

        private uint m_hNeverwinterNights = 0;
        private Module m_module;

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



    public class LogEntries
    {
        public LogEntries(uint hLogEntryCollection, uint indx)
        {
            m_hLogEntryCollection = hLogEntryCollection;
            m_indx = indx;
            MoeSzyslakLibrary.InvokeHandle(m_hLogEntryCollection, string.Format("{0} get text", m_indx));
            m_text = MoeSzyslakLibrary.GetReturnString(m_hLogEntryCollection);
        }

        private uint m_hLogEntryCollection;
        private uint m_indx;
        private string m_text;
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
            foreach (KeyValuePair<string, string> kvp in m_testValues)
            {
                MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set \"{0}\" \"{1}\"", kvp.Key, kvp.Value));
            }

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

        public void UnitTest()
        {
            Database db = new Database();

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "UnitTest");
            db.Dispose();
        }

        private uint m_hObj;
        private Dictionary<string, string> m_testValues = new Dictionary<string, string>();
        private bool m_bPassed = true;


    }

    public class VLVariable
    {
        public enum VAR_TYPE
        {
            TYPE_STRING = 0,
            TYPE_BOOL = 3
        };

        public VLVariable()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(VLVariable.ClassID);

        }

        ~VLVariable()
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

        public void Commit()
        {
            if (m_type == VAR_TYPE.TYPE_BOOL)
            {
                if (m_bVal)
                    MoeSzyslakLibrary.InvokeHandle(m_hObj, "set bool true");
                else
                    MoeSzyslakLibrary.InvokeHandle(m_hObj, "set bool false");

            }

            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set display \"{0}\"", m_displayName));
        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get type");
            m_type = (VAR_TYPE)Convert.ToUInt32(MoeSzyslakLibrary.GetReturnString(m_hObj));

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get value");
            m_str = MoeSzyslakLibrary.GetReturnString(m_hObj);

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get display");
            m_displayName = MoeSzyslakLibrary.GetReturnString(m_hObj);

            if (m_type == VAR_TYPE.TYPE_BOOL)
            {
                m_bVal = m_str == "TRUE";
            }
        }

        private uint m_hObj = 0;
        public const uint ClassID = 87153;
        VAR_TYPE m_type = VAR_TYPE.TYPE_STRING;

        public bool AsBool
        {
            set
            {
                m_bVal = value;
                m_type = VAR_TYPE.TYPE_BOOL;
            }
            get { return m_bVal; }
        }
        private bool m_bVal = false;

        public string AsString
        {
            set { m_str = value; }
            get { return m_str; }
        }
        private string m_str = "";


        public string DisplayName
        {
            set { m_displayName = value; }
            get { return m_displayName; }
        }
        private string m_displayName = "";
    }
}

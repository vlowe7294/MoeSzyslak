using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Collections.Generic;

namespace MoeConsole
{
    public class LogEntries
    {
        public LogEntries(uint hLogEntryCollection, uint indx)
        {
            m_hLogEntryCollection = hLogEntryCollection;
            m_indx = indx;
            MoeSzyslakLibrary.InvokeHandle(m_hLogEntryCollection, string.Format("{0} get text", m_indx));
        }

        private uint m_hLogEntryCollection;
        private uint m_indx;
        private string m_text;
    }

    public class Testing
    {
        public Testing()
        {
            m_hObj= MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.TESTING);
            
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
            
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Passed");
            

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

    public class VLVariable
    {
        public enum VAR_TYPE
        {
            TYPE_STRING = 0,
            TYPE_INT    = 2,
            TYPE_BOOL   = 3
        };

        public VLVariable()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);

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

        public void UnitTest()
        {
            Testing tst = new Testing();
            Console.WriteLine("Test Values");
            Console.WriteLine("Variable Display Name:  test variable");
            Console.WriteLine("Variable value:  0,0,29,23\n");

            tst.SetTestValue("name", "test variable");
            tst.SetTestValue("value", "0,0,29,23");
            tst.RunTest(m_classID);
            tst.Dispose();
        }

        public void Update()
        {
            
        }

        private uint m_hObj = 0;
        private const uint m_classID = 87153;
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

        public void SetAsInt(int ndx, int nVal)
        {
            if (m_fValues.Length <= ndx)
            {
                double[] fNew = new double[ndx * 2];
                m_fValues.CopyTo(fNew, 0);
                m_fValues = fNew;
            }

            m_fValues[ndx] = (double)nVal;
            m_type = VAR_TYPE.TYPE_INT;

        }

        private double[] m_fValues = new double[10];
    }

    public class Module
    {
        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        private string m_name = "New Module";

    }

    public class CommGeneralPage
    {
        public CommGeneralPage(uint hIOS)
        {
            m_hIOS = hIOS;
            
        }

        public void UnitTest()
        {
            this.BackupAudio = true;
            Commit();

        }

        public void Commit()
        {
            if (m_bBackupAudio)
                MoeSzyslakLibrary.InvokeHandle(m_hIOS, "\"Comm General Page\" set \"Backup Audio Enabled\" true");
            else
                MoeSzyslakLibrary.InvokeHandle(m_hIOS, "\"Comm General Page\" set \"Backup Audio Enabled\" false");

            if (m_bAuralCuesEnabled)
                MoeSzyslakLibrary.InvokeHandle(m_hIOS, "\"Comm General Page\" set \"Aural Cues Enabled\" true");
            else
                MoeSzyslakLibrary.InvokeHandle(m_hIOS, "\"Comm General Page\" set \"Aural Cues Enabled\" false");

        }

        private uint m_hIOS;

        public bool BackupAudio
        {
            get { return m_bBackupAudio; }
            set { m_bBackupAudio = value; }
        }
        private bool m_bBackupAudio = false;

        public bool AuralCuesEnabled
        {
            get { return m_bAuralCuesEnabled; }
            set { m_bAuralCuesEnabled = value; }
        }
        private bool m_bAuralCuesEnabled = false;

        

    }

    public class IOS
    {
        public IOS()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
            m_commGeneralPage = new CommGeneralPage(m_hObj);
        }

        ~IOS()
        {
            Dispose();
        }

        public void Dispose()
        {

            if (m_hObj > 0)
            {
                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hObj);
                m_hObj = 0;
                m_commGeneralPage = null;
            }
        }

        static public void UnitTest()
        {
            Testing tst = new Testing();

            tst.RunTest(m_classID);            
            tst.Dispose();

            /*IOS ios = new IOS();
            ios.m_commGeneralPage.UnitTest();

            ios.Dispose();*/
        }

        private uint m_hObj;
        private const uint m_classID = 306758;
        private CommGeneralPage m_commGeneralPage;
    }

    public class Finance
    {
        public Finance()
        {
            
        }


        static public void UnitTest()
        {
            Testing tst = new Testing();
            tst.MemoryCheck = true;
            
            tst.SetTestValue("count", "500");
            tst.SetTestValue("object type", "87153");
            tst.SetTestValue("command", "206 set value XORN");
            tst.SetTestValue("get", "206");
            tst.SetTestValue("remove", "207");

            tst.RunTest(m_classID);
            tst.Dispose();

            
        }

        private uint m_hObj;
        private const uint m_classID = 346141;
    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();

                IOS.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            
        }
    }
}

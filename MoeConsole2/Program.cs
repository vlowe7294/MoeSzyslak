using MoeConsole;
using System;
using System.Data;
using System.Drawing;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Xml.Linq;
using static MoeConsole.TripPlanner;
using static System.Collections.Specialized.BitVector32;
using static System.Net.Mime.MediaTypeNames;

namespace MoeConsole
{
    public class Transaction
    {

    }


    public class Account
    {
        public Account(uint hFinanceObj)
        {
            m_hObj = hFinanceObj;

        }

        public string GetVarString()
        {
            return string.Format("accountID={0}&accountName={1}&openingBalance={2}", m_accountID, m_accountName, m_openingBalance);
        }

        public void Print()
        {
            Console.WriteLine("Account Name:\t{0}", m_accountName);

        }

        public void Commit()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("account set name \"{0}\"", m_accountName));
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("account set balance {0}", m_openingBalance));
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "account transaction count");

            int nTran = int.Parse(MoeSzyslakLibrary.GetReturnString(m_hObj));

            while (nTran < m_transactions.Count)
            {
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "account AddTransaction");
                nTran++;
            }
        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "account get name");
            m_accountName = MoeSzyslakLibrary.GetReturnString(m_hObj);
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "account get balance");
            m_openingBalance = decimal.Parse(MoeSzyslakLibrary.GetReturnString(m_hObj));
        }

        public void NewTransaction()
        {
            m_transactions.Add(new Transaction());
        }


        public string AccountID
        {
            set { m_accountID = value; }
        }
        private string m_accountID = "";

        public string AccountName
        {
            set { m_accountName = value; }
        }
        private string m_accountName = "New Account";

        public decimal OpeningBalance
        {
            set { m_openingBalance = value; }
        }
        private decimal m_openingBalance = 0.0M;

        private uint m_hObj;
        private List<Transaction> m_transactions = new List<Transaction>();
    }

    public class Finance
    {
        public Finance()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
            m_account = new Account(m_hObj);
        }

        ~Finance()
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

        public void Print()
        {
            Update();
            m_account.Print();
        }

        public void Commit()
        {
            m_account.Commit();
        }

        public void Update()
        {
            m_account.Update();
        }


        public static void UnitTest()
        {
            Finance fnc = new Finance();
            Testing tst = new Testing();

            tst.SetTestValue("count", "1");
            tst.SetTestValue("object type", "87153");
            tst.SetTestValue("command", "get count");
            tst.SetTestValue("account name", "Wells Fargo");
            tst.SetTestValue("balance", "1200.00");

            fnc.theAccount.AccountID = "3997870419";
            fnc.theAccount.AccountName = "Wells Fargo";
            fnc.theAccount.OpeningBalance = 1200.00M;

            fnc.theAccount.NewTransaction();
            fnc.Commit();

            fnc.Print();
            fnc.Dispose();

            // tst.RunTest(m_classID);            

            tst.Dispose();
        }

        public Account theAccount
        {
            get { return m_account; }
        }
        private Account m_account;

        private const uint m_classID = 346141;
        private uint m_hObj;
    }


    public class User : IDisposable
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("3B1537E5-9E5D-4053-8510-1CEB51543F32")]
        private interface IUSER 
        {
            
        }

        public User(IntPtr iUnk)
        {
            m_iunk = iUnk;
            m_iUser = (IUSER)Marshal.GetObjectForIUnknown(m_iunk);
        }

        ~User() => Dispose(false);

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            if (m_iUser != null)
            {
                Marshal.ReleaseComObject(m_iUser);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iUser = null;
                m_iunk = IntPtr.Zero;
            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private IUSER m_iUser;



        public void Commit()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set \"Login Name\" \"{0}\"", m_userName));
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set Password \"{0}\"", m_password));
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set Email \"{0}\"", m_email));

        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get \"Login Name\"");
            m_userName = MoeSzyslakLibrary.GetReturnString(m_hObj);
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Password");

            m_password = MoeSzyslakLibrary.GetReturnString(m_hObj);
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get \"Is Logged In\"");
            m_bIsLoggedIn = MoeSzyslakLibrary.GetReturnString(m_hObj) == "TRUE";

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Email");
            m_email = MoeSzyslakLibrary.GetReturnString(m_hObj);
        }

        public static void UnitTest()
        {
            Testing tst = new Testing();

            tst.RunTest(m_classID);    
            tst.Dispose();
        }

        public void Print()
        {
            Console.WriteLine("User Name:\t{0}", m_userName);
            Console.WriteLine("Password:\t{0}", m_password);
            Console.WriteLine("Is Logged In:\t{0}", m_bIsLoggedIn);
            Console.WriteLine("Email:\t{0}", m_email);
        }

        public void Save()
        {
            Database db = new Database();

            Table tbl = db.GetTable("users");

            db.Write("users.db");

        }

        public void Login()
        {
            Commit();
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "Login");
            Update();
        }

        private const uint m_classID = 325850;
        private uint m_hObj;   

        public string Name
        {
            set { m_userName = value; }
            get { return m_userName; }
        }
        private string m_userName = "";

        public string Password
        {
            set { m_password = value; }
            get { return m_password; }
        }
        private string m_password = "";

        public bool IsLoggedIn
        {
            get { return m_bIsLoggedIn; }
        }
        private bool m_bIsLoggedIn = false;

        public string Email
        {
            set { m_email = value; }
            get { return m_email; }
        }
        
        private string m_email = "";

    }

    public class UserService : IDisposable
    {
        static public uint classID = 241805;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("39663C08-D7C0-4CB2-B4F0-0E1F2445A5B0")]
        private interface IUSERSERVICE
        {
            void GetUser([MarshalAs(UnmanagedType.LPWStr)] string strName, ref IntPtr iUsr);
            void UnitTest();            
        }

        public UserService()
        {
            MoeSzyslakLibrary.CreateMoeSzyslakInterface(classID, ref m_iunk);
            m_iUserService = (IUSERSERVICE)Marshal.GetObjectForIUnknown(m_iunk);
        }

        ~UserService() => Dispose(false);

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            if (m_iUserService != null)
            {
                Marshal.ReleaseComObject(m_iUserService);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iUserService = null;
                m_iunk = IntPtr.Zero;
            }                
        }

        public User GetUser()
        {
            IntPtr iUser = IntPtr.Zero;
            m_iUserService.GetUser("Vaughn", ref iUser);

            if (iUser == IntPtr.Zero)
                return null;

            return new User(iUser);
        }

        public static void UnitTest()
        {
            using (UserService us = new UserService())
            {
                using (User u = us.GetUser())
                {

                }

                us.m_iUserService.UnitTest();
            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private IUSERSERVICE m_iUserService;

    }


    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Console.WriteLine(MoeSzyslakLibrary.Version);
                UserService.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
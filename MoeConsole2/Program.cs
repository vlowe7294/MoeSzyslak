using MoeConsole;
using System;
using System.Data;
using System.Drawing;
using System.Reflection.Emit;
using System.Security.Principal;
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


    public class User
    {
        public User()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
        }

        ~User()
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
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set \"Login Name\" \"{0}\"", m_userName));
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set Password \"{0}\"", m_password));

        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get \"Login Name\"");
            m_userName = MoeSzyslakLibrary.GetReturnString(m_hObj);
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Password");

            m_password = MoeSzyslakLibrary.GetReturnString(m_hObj);
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get \"Is Logged In\"");
            m_bIsLoggedIn = MoeSzyslakLibrary.GetReturnString(m_hObj) == "TRUE";
        }

        public static void UnitTest()
        {
            User usr = new User();
            Testing tst = new Testing();

            usr.Name = "Vaughn";
            usr.Password = "ZFyZH8DuKemv";
            usr.Login();
            //usr.Print();
            
            tst.RunTest(m_classID);    
            tst.Dispose();
            usr.Dispose();
        }

        public void Print()
        {
            Console.WriteLine("User Name:\t{0}", m_userName);
            Console.WriteLine("Password:\t{0}", m_password);
            Console.WriteLine("Is Logged In:\t{0}", m_bIsLoggedIn);
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

    }


    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Console.WriteLine(MoeSzyslakLibrary.Version);
                User.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
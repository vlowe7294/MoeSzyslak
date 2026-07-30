using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MoeConsole
{
    public class User : IDisposable
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("3B1537E5-9E5D-4053-8510-1CEB51543F32")]
        private interface IUSER
        {
            void Login([MarshalAs(UnmanagedType.LPWStr)] string strNme, [MarshalAs(UnmanagedType.LPWStr)] string strPwd);
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

        public void Login(string userName, string password)
        {
            m_iUser.Login(userName, password);
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private IUSER m_iUser;



        public void Commit()
        {
            

        }

        public static void UnitTest()
        {
            Testing tst = new Testing(IntPtr.Zero);

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

        public User GetUser(string usrNme)
        {
            IntPtr iUser = IntPtr.Zero;
            m_iUserService.GetUser(usrNme, ref iUser);

            if (iUser == IntPtr.Zero)
                return null;

            return new User(iUser);
        }

        public static void UnitTest()
        {
            using (UserService us = new UserService())
            {
                using (User u = us.GetUser("Vaughn"))
                {
                    u.Login("Vaughn", "ZFyZH8DuKemv");
                }

                us.m_iUserService.UnitTest();
            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private IUSERSERVICE m_iUserService;

    }

}

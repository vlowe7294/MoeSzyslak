using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MoeSyzslakFormsApp2
{
    public partial class MoeUserControl : UserControl
    {
        public MoeUserControl()
        {
            m_user = new User();
            InitializeComponent();
            NameTextBox.Text = "Vaughn";
            PasswordTextBox .Text = "ZFyZH8DuKemv";
        }

        public User TheUser
        {
            get { return m_user; }
        }
        private User m_user;

        private void OnLogin(object sender, EventArgs e)
        {
            m_user.Name = NameTextBox.Text;
            m_user.Password = PasswordTextBox.Text;
            m_user.Login();

            if (m_user.IsLoggedIn)
            {
                this.Visible = false;

                var parent = this.Parent as MainForm;
                if (parent != null)
                {
                    parent.EditUser();
                }
            }
        }
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

        public static void UnitTest(TestControl tstCntl)
        {
            int nTests = 2;
            User usr;


            tstCntl.Message("Testing User Object");

            tstCntl.TestValue("Login Name", "Vaughn");
            tstCntl.TestValue("Password", "ZFyZH8DuKemv");
            tstCntl.TestValue("Is Logged In", true);


            for (int i = 0; i < nTests; i++)
            {
                usr = new User();
                tstCntl.Message(string.Format("Running test {0} of {1}", i + 1, nTests));
                usr.Name = tstCntl.TestValue("Login Name").ToString();
                usr.Password = tstCntl.TestValue("Password").ToString();

                usr.Login();
                tstCntl.VerifyVariable("Is Logged In", usr.IsLoggedIn);
                // tstCntl.Verify(usr.IsLoggedIn, "Failed to login");

                tstCntl.VerifyVariable("Login Name", usr.Name);
                tstCntl.VerifyVariable("Password", usr.Password);

                tstCntl.TestValue("Login Name", "Nevin");
                tstCntl.TestValue("Password", "nevin5");
                tstCntl.TestValue("Is Logged In", false);
                usr.Dispose();
            }

            tstCntl.Report();

        }

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

}

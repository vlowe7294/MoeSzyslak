using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using static System.Net.Mime.MediaTypeNames;
using static System.Runtime.InteropServices.JavaScript.JSType;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.TaskbarClock;

namespace MoeSyzslakFormsApp2
{
    public partial class TestControl : UserControl
    {
        public TestControl()
        {
            InitializeComponent();
            ClassListBox.SelectedIndex = 1;
        }

        public void Message(string strMsg)
        {
            long dur = m_watch.ElapsedMilliseconds;
            m_logEntries.Add(new LogEntry(dur, strMsg));

        }

        public void Verify(bool bVal, string szMsg)
        {
            if (!bVal)
                Message(szMsg);

        }

        private void OnRunTest(object sender, EventArgs e)
        {
            try
            {
                m_logEntries.Clear();
                m_testTime = DateTime.Now;
                Message(string.Format("Tested on {0}", m_testTime));
                m_watch = Stopwatch.StartNew();
                ResultLabel.ForeColor = Color.DarkGreen;
                m_bPassed = true;


                if (ClassListBox.SelectedItem.ToString() == "User")
                {
                    User.UnitTest(this);
                    m_watch.Stop();

                    if (m_bPassed)
                    {
                        ResultLabel.Text = "Result:  PASS";
                    }
                    else
                    {
                        ResultLabel.ForeColor = Color.Red;
                        ResultLabel.Text = "Result:  FAILED";
                    }
                    return;
                }

                int nTests = 2;

                Message("Testing Object Self Unit Test");
                Message(string.Format("Log Entry Version 1.3.6.{0}", LogEntry.Version - 1530));
                Message(string.Format("Tested on {0}", m_testTime));


                for (int i = 0; i < nTests; i++)
                {
                    m_watch = Stopwatch.StartNew();
                    Message(string.Format("Running test {0} of {1}", i + 1, nTests));
                    RunTest(this.GetType());
                    m_watch.Stop();
                }                
            }
            catch (Exception ex)
            {
                Verify(false, "Unhandled Exception: " + ex.Message);
            }

            Report();
        }

        private void RunTest(Type objToTest)
        {
            

            SelfTest();
            
        }

        private void SelfTest()
        {
            m_bPassed = true;
            Verify(m_logEntries.Count > 2, "Log entry count < 2");
        }

        public void Report()
        {
            foreach (LogEntry entry in m_logEntries)
            {
                ResultsListView.Items.Add(new ListViewItem(new string[] { entry.Duration.ToString(), entry.Message }));
            }   
        }

        public object TestValue(string nme)
        {
            if (!m_testValues.ContainsKey(nme))
                return "";
            else
                return m_testValues[nme];
        }

        public void TestValue(string nme, object val)
        {
            m_testValues[nme] = val;
        }

        public void VerifyVariable(string varName, object var)
        {
            object localVar = null;
            string s1 = "", s2 = var.ToString();

            if (m_testValues.ContainsKey(varName))
            {
                localVar = m_testValues[varName];

            }

            if (localVar != null)
            {
                s1 = localVar.ToString();
            }

            if (s1 != s2)
            {
                string l = varName + " does not match " + s2;
                Verify(false, l);
            }

        }

        private Dictionary<string, object> m_testValues = new Dictionary<string, object>();
        private bool m_bMemoryCheck = false;
        private bool m_bPassed = false;
        private List<LogEntry> m_logEntries = new List<LogEntry>();
        private DateTime m_testTime = DateTime.Now;
        private Stopwatch m_watch = new Stopwatch();
        public static uint Version = 1587;
    }

    public class LogEntry
    {
        public LogEntry(long tme, string txt)
        {
            m_duration = tme;
            m_message = txt;
        }

        public long Duration { get { return m_duration; } }
        private long m_duration;

        public string Message { get { return m_message; } }
        private string m_message;

        public static uint Version = 1573;

    }
}

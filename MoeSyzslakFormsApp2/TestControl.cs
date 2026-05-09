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
            ClassListBox.SelectedIndex = 0;
        }

        public void Message(string strMsg)
        {
            long dur = m_watch.ElapsedMilliseconds;
            m_logEntries.Add(new LogEntry(dur, strMsg));

        }

        public void Verify(bool bVal, string szMsg)
        {

        }

        private void OnRunTest(object sender, EventArgs e)
        {
            TestValue("message", "test message 1");
            m_watch = Stopwatch.StartNew();
            RunTest(this.GetType());

            m_watch.Stop();

        }

        private void RunTest(Type objToTest)
        {
            m_logEntries.Clear();
            ResultLabel.ForeColor = Color.DarkGreen;

            try
            {
                if (objToTest == typeof(TestControl))
                    SelfTest();
                else
                    throw new Exception("No test defined for type " + objToTest.ToString());
            }
            catch (Exception ex)
            {
                Message(ex.Message);
                m_bPassed = false;
            }

            Report();

            if (m_bPassed)
            {
                ResultLabel.Text = "Result:  PASS";
            }
            else
            {
                ResultLabel.ForeColor = Color.Red;
                ResultLabel.Text = "Result:  FAILED";
            }
        }

        private void SelfTest()
        {
            Message("Testing Object Self Unit Test");
            Message(string.Format("Log Entry Version 1.3.6.{0}", LogEntry.Version - 1530));
            m_bPassed = true;

            m_testTime = DateTime.Now;
            Message(string.Format("Tested on {0}", m_testTime));

            Message(TestValue("message").ToString());
            Verify(m_logEntries.Count > 2, "Log entry count < 2");
        }

        private void Report()
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

        private Dictionary<string, object> m_testValues = new Dictionary<string, object>();
        private bool m_bMemoryCheck = false;
        private bool m_bPassed = false;
        private List<LogEntry> m_logEntries = new List<LogEntry>();
        private DateTime m_testTime = DateTime.Now;
        private Stopwatch m_watch = new Stopwatch();
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

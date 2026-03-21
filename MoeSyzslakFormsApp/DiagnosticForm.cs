using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace MoeSyzslakFormsApp
{
    public partial class DiagnosticForm : Form
    {
        public DiagnosticForm()
        {
            InitializeComponent();
            VersionLabel.Text = MoeSzyslakLibrary.Version;
            
            m_hObj = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.TESTING);
            TestObjectListBox.SelectedIndex = 0;
        }

        public void OnUnitTest(object sender, EventArgs e)
        {
            uint nClassID = (uint)MoeSzyslakLibrary.CLASSID.TESTING;
            SetTestValue("message", "Testing Object Self Unit Test");

            foreach (KeyValuePair<string, string> kvp in m_testValues)
            {
                MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set \"{0}\" \"{1}\"", kvp.Key, kvp.Value));
            }


            if (MemoryCheckBox.Checked)
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" TRUE");
            else
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "set \"Memory Check\" FALSE");

            if (!MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("RunTest {0}", nClassID)))
                ErrorLabel.Text = MoeSzyslakLibrary.LastError;

            OutputTextBox.Text = MoeSzyslakLibrary.GetReturnString(m_hObj, 1000);

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "get Passed");
            m_bPassed = MoeSzyslakLibrary.GetReturnString(m_hObj) == "TRUE";

            if (!m_bPassed)
                this.ErrorMessage = "Unit test failed.";           
            

            MoeSzyslakLibrary.InvokeHandle(m_hObj, "log get count");
            uint nCount = Convert.ToUInt32(MoeSzyslakLibrary.GetReturnString(m_hObj)), i = 0;

            while (nCount > 0)
            {
                nCount--;
            }
        }

        public void SetTestValue(string name, string val)
        {
            m_testValues[name] = val;
        }

        private uint m_hObj = 0;
        private List<LogEntries> m_logEntries = new List<LogEntries>();
        private bool m_bPassed = true;
        private Dictionary<string, string> m_testValues = new Dictionary<string, string>();

        public string ErrorMessage
        {
            set
            {
                ErrorLabel.Text = value;
            }
        }
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
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MoeSyzslakFormsApp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            MainPropertyGrid.SelectedObject = m_variable;
        }

        private void OnTestButton(object sender, EventArgs e)
        {
            if (m_diagnosticFrm == null)
                m_diagnosticFrm = new DiagnosticForm();

            if (!m_diagnosticFrm.Visible)
                m_diagnosticFrm.Show();
            else
                m_diagnosticFrm.Visible = false;

        }

        DiagnosticForm m_diagnosticFrm;
        VLVariable m_variable = new VLVariable();
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

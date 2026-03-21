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
    public partial class IOSForm : Form
    {
        public IOSForm()
        {
            InitializeComponent();
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
            theCommGeneralPage.HandleIOS = m_hObj;
        }

        private uint m_hObj;
        private const uint m_classID = 306758;
    }
}

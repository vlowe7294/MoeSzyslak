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
    public partial class CommGeneralPage : UserControl
    {
        public CommGeneralPage()
        {
            InitializeComponent();
        }

        public uint HandleIOS = 0;

        private void OnApply(object sender, EventArgs e)
        {
            if (BackupAudio.Checked)
                MoeSzyslakLibrary.InvokeHandle(HandleIOS, "\"Comm General Page\" set \"Backup Audio Enabled\" true");
            else
                MoeSzyslakLibrary.InvokeHandle(HandleIOS, "\"Comm General Page\" set \"Backup Audio Enabled\" false");

        }
    }
}

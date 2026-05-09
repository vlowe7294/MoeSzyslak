using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MoeSyzslakFormsApp2
{
    public partial class UserEditControl : UserControl
    {
        public UserEditControl()
        {
            InitializeComponent();
        }

        public void SetUser(User user)
        {
            m_user = user;
            UserNameValue.Text = m_user.Name;
        }  
        private User m_user;
    }

    
}

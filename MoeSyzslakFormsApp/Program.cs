using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MoeSyzslakFormsApp
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            DiagnosticForm diag = new DiagnosticForm();

            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Application.Run(diag);
            }
            catch (Exception exc)
            {
                diag.ErrorMessage = exc.Message;
                diag.ShowDialog();
            }
        }
    }
}

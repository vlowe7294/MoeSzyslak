namespace MoeSyzslakFormsApp2
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            try
            {
                VersionCheck();
                ApplicationConfiguration.Initialize();
                Application.Run(new MainForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        static void VersionCheck()
        {
            if (TestControl.Version != m_testVersion)
                throw new Exception(string.Format("Test Control version required:  1.3.6.{0}.  Actual version is :  1.3.6.{1}",
                    m_testVersion - 1530, TestControl.Version - 1530));

        }

        static private uint m_testVersion = 1587;
    }
}
namespace MoeConsole
{
    public class Database
    {
        public Database()
        {
            m_hObj = MoeSzyslakLibrary.CreateHandle(MoeSzyslakLibrary.CLASSID.DATABASE);
        }

        ~Database()
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

        private uint m_hObj;
    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Console.WriteLine(MoeSzyslakLibrary.Version);
                Testing.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
using System.Runtime.InteropServices;
using System.Text;

namespace MoeConsole
{
    class Site
    {
        public Site(IntPtr iunk)
        {
            m_iunk = iunk;
        }

        public void Dispose()
        {
            if (m_iunk != IntPtr.Zero)
            { 
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;                
            }
        }

        private IntPtr m_iunk;

    }

    class CampArea
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("764BAFFA-9281-4610-8683-E0B8B0841D0F")]
        private interface ICAMPAREA
        {
            void GetName([MarshalAs(UnmanagedType.BStr)] out string strNme);
	        void SetName([MarshalAs(UnmanagedType.LPWStr)] string strNme);
            void GetSite(int ndx, ref IntPtr iSite);
        }
        public CampArea(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iCampArea = (ICAMPAREA)Marshal.GetObjectForIUnknown(m_iunk);
        }

        public void Dispose()
        {
            foreach (Site ste in m_sites)
            {
                ste.Dispose();
            }

            m_sites.Clear();

            if (m_iCampArea != null)
            {
                Marshal.ReleaseComObject(m_iCampArea);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
                m_iCampArea = null;
            }
        }

        public void Update()
        {
            int i = 0;
            IntPtr iSite = IntPtr.Zero;

            m_iCampArea.GetName(out m_strName);

            foreach (Site ste in m_sites)
            {
                ste.Dispose();
            }

            m_sites.Clear();
            m_iCampArea.GetSite(0, ref iSite);

            while (iSite != IntPtr.Zero)
            {
                Site ste = new Site(iSite);
                m_sites.Add(ste);
                i++;
                m_iCampArea.GetSite(i, ref iSite);
            }
        }

        public string Name
        {
            get
            {
                return m_strName;
            }            
        }

        private IntPtr m_iunk;
        private ICAMPAREA m_iCampArea;
        private List<Site> m_sites = new List<Site>();
        string m_strName = "";
    }

    class CampSight : IDisposable
    {
        static public uint classID = 929959;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("583AFD09-699D-42C2-98FE-753E75422F16")]
        private interface ICAMPSIGHT
        {
            void GetTester(ref IntPtr iTester);
            void AddSite([MarshalAs(UnmanagedType.LPWStr)] string szArea, [MarshalAs(UnmanagedType.LPWStr)] string szSite, double lat, double lon);
            void GetArea(int ndx, ref IntPtr iArea);
            void UnitTest();
        }

        public CampSight()
        {
            IntPtr iTester = IntPtr.Zero;
            MoeSzyslakLibrary.CreateMoeSzyslakInterface(classID, ref m_iunk);
            m_iCampSight = (ICAMPSIGHT)Marshal.GetObjectForIUnknown(m_iunk);

            m_iCampSight.GetTester(ref iTester);
            m_tester = new Testing(iTester);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            m_tester.Dispose();

            foreach (CampArea ar in m_campAreas)
            {
                ar.Dispose();
            }

            m_campAreas.Clear();

            Marshal.ReleaseComObject(m_iCampSight);
            MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
            m_iCampSight = null;
            m_iunk = IntPtr.Zero;
        }

        public void AddSite(string strArea, string strSite, double lat, double lon)
        {
            m_iCampSight.AddSite(strArea, strSite, lat, lon);
        }

        public void Update()
        {
            int i = 0;
            string nme;
            IntPtr iArea = IntPtr.Zero;

            foreach(CampArea ar in m_campAreas)
            {
                ar.Dispose();
            }

            m_campAreas.Clear();
            m_iCampSight.GetArea(0, ref iArea);

            while (iArea != IntPtr.Zero)
            {
                CampArea ar = new CampArea(iArea);
                m_campAreas.Add(ar);
                ar.Update();

                i++;
                m_iCampSight.GetArea(i, ref iArea);
            }
        }

        public static void UnitTest()
        {
            using (CampSight cs = new CampSight())
            {
                Testing tst = cs.theTester;
                try
                {
                    cs.m_tester.Message("Camp Sight Unit Test", Testing.DEBUG_LEVEL.DEBUG_INFO, "CampSight");
                    cs.AddSite("Dinosaur Valley State Park", "Laham Mill #14", 32.251301564676666, -97.8112404606453);
                    cs.Update();

                    tst.Verify(cs.m_campAreas[0].Name == "Dinosaur Valley State Park", "Area name mismatch", (int)Testing.DEBUG_LEVEL.DEBUG_CRITICAL, "CampSight");

                    Console.WriteLine("Camp Area Name: " + cs.m_campAreas[0].Name);

                    cs.m_iCampSight.UnitTest();
                }
                catch (Exception ex)
                {
                    tst.Verify(false, ex.Message, (int)Testing.DEBUG_LEVEL.DEBUG_CRITICAL, "CampSight");
                }

                tst.Report();
            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private ICAMPSIGHT m_iCampSight;

        public Testing theTester { get { return m_tester; } }
        private Testing m_tester;

        private static StringBuilder m_sb = new StringBuilder(1024);
        private List<CampArea> m_campAreas = new List<CampArea>();

    }
}
using MoeConsole;
using System;
using System.Data;
using System.Drawing;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Xml.Linq;
using static MoeConsole.TripPlanner;
using static System.Collections.Specialized.BitVector32;
using static System.Net.Mime.MediaTypeNames;
using System.Collections.Generic;

namespace MoeConsole
{

    class CampArea
    {
        public CampArea(string nme)
        {
            m_name = nme;
        }

        private string m_name;
    }




    class CampSight : IDisposable
    {
        static public uint classID = 929959;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("583AFD09-699D-42C2-98FE-753E75422F16")]
        private interface ICAMPSIGHT
        {
            void GetTester(ref IntPtr iTester);
            void AddSite([MarshalAs(UnmanagedType.LPWStr)] string szArea, [MarshalAs(UnmanagedType.LPWStr)] string szSite, double lat, double lon);
            void GetArea(uint ndx, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder szName, uint nLen);
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

            m_campArea.Clear();
            m_iCampSight.GetArea(0, m_sb, (uint)m_sb.Capacity);
            nme = m_sb.ToString();

            while (!string.IsNullOrEmpty(nme))
            {
                CampArea ca = new CampArea(nme);
                m_campArea.Add(ca);

                i++;
                m_iCampSight.GetArea((uint)i, m_sb, (uint)m_sb.Capacity);
                nme = m_sb.ToString();
            }

        }

        public static void UnitTest()
        {
            using (CampSight cs = new CampSight())
            {
                cs.m_tester.Message("Camp Sight Unit Test", Testing.DEBUG_LEVEL.DEBUG_INFO);
                cs.AddSite("Dinosaur Valley State Park", "Laham Mill #14", 32.251301564676666, -97.8112404606453);
                cs.Update();
                cs.m_iCampSight.UnitTest();
                Console.WriteLine(cs.m_tester.Report());

            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private ICAMPSIGHT m_iCampSight;

        public Testing theTester { get { return m_tester; } }
        private Testing m_tester;

        private static StringBuilder m_sb = new StringBuilder(1024);
        private List<CampArea> m_campArea = new List<CampArea>();

    }


    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Testing.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
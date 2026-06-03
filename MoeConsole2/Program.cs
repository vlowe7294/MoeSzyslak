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

namespace MoeConsole
{

    public class AstralWorkshop : IDisposable
    {
        static public uint classID = 133671;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("F43BA252-2FCA-41A2-9CFA-22A5D87C584C")]
        private interface IASTRALWORKSHOP
        {
            void Command([MarshalAs(UnmanagedType.LPWStr)] string strCmd, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder strRet, uint nLen);
            void UnitTest();
        }

        public AstralWorkshop()
        {
            MoeSzyslakLibrary.CreateMoeSzyslakInterface(classID, ref m_iunk);
            m_iAstralWorkshop = (IASTRALWORKSHOP)Marshal.GetObjectForIUnknown(m_iunk);
        }

        ~AstralWorkshop() => Dispose(false);

        public static void UnitTest()
        {
            using (AstralWorkshop aw = new AstralWorkshop())
            {
                Console.WriteLine("Create");
                Console.WriteLine(aw.Command("Create"));

                Console.WriteLine("Rowan");
                Console.WriteLine(aw.Command("Rowan"));

                Console.WriteLine("1");
                Console.WriteLine(aw.Command("1"));

                Console.WriteLine("2");
                Console.WriteLine(aw.Command("2"));

                Console.WriteLine("exit");
                Console.WriteLine(aw.Command("exit"));

                //aw.m_iAstralWorkshop.UnitTest();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            Marshal.ReleaseComObject(m_iAstralWorkshop);
            MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
            m_iAstralWorkshop = null;
            m_iunk = IntPtr.Zero;
        }

        string Command(string cmd)
        {
            m_iAstralWorkshop.Command(cmd, m_sb, (uint)m_sb.Capacity);
            return m_sb.ToString();
        }   

        private IntPtr m_iunk = IntPtr.Zero;
        private IASTRALWORKSHOP m_iAstralWorkshop;
        private StringBuilder m_sb = new StringBuilder(1024);
    }




    class CampSight : IDisposable
    {
        static public uint classID = 929959;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("583AFD09-699D-42C2-98FE-753E75422F16")]
        private interface ICAMPSIGHT
        {
            void GetTester(ref IntPtr iTester);
            void AddSite([MarshalAs(UnmanagedType.LPWStr)] string szArea, [MarshalAs(UnmanagedType.LPWStr)] string szSite, double lat, double lon);
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

        public static void UnitTest()
        {
            using (CampSight cs = new CampSight())
            {
                cs.m_tester.Message("Camp Sight Unit Test");
                cs.AddSite("Dinosaur Valley State Park", "Laham Mill #14", 32.251301564676666, -97.8112404606453);
                cs.m_iCampSight.UnitTest();

            }
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private ICAMPSIGHT m_iCampSight;
        private Testing m_tester;

    }


    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                Console.WriteLine(MoeSzyslakLibrary.Version);
                CampSight.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MoeConsole
{
    public class Area
    {
        public Area(IntPtr iunk) 
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

        IntPtr m_iunk = IntPtr.Zero;
    }

    public class AstralWorkshop : IDisposable
    {
        static public uint classID = 133671;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("F43BA252-2FCA-41A2-9CFA-22A5D87C584C")]
        private interface IASTRALWORKSHOP
        {
            void Command([MarshalAs(UnmanagedType.LPWStr)] string strCmd, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder strRet, uint nLen);
            void NewArea(ref IntPtr iArea, ref int ndx, [MarshalAs(UnmanagedType.LPWStr)] string strName, int nDanger);
            void Export([MarshalAs(UnmanagedType.LPWStr)] string strPath);
            void Save([MarshalAs(UnmanagedType.LPWStr)] string strPath);
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
                aw.NewArea("Graveyard", 2);
                aw.Export("astralworkshop.xml");
                aw.Save("astralworkshop.dat");
                aw.m_iAstralWorkshop.UnitTest();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            foreach (Area a in m_areas)
                a.Dispose();

            m_areas.Clear();
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

        public Area NewArea(string strName, int nDanger)
        {
            IntPtr iArea = IntPtr.Zero;
            int ndx = 0;

            m_iAstralWorkshop.NewArea(ref iArea, ref ndx, strName, nDanger);
            Area a = new Area(iArea);
            m_areas.Add(a);
            return a;

        }

        public void Export(string strPath)
        {
            m_iAstralWorkshop.Export(strPath);
        }

        public void Save(string strPath)
        {
            m_iAstralWorkshop.Save(strPath);
        }

        private IntPtr m_iunk = IntPtr.Zero;
        private IASTRALWORKSHOP m_iAstralWorkshop;
        private StringBuilder m_sb = new StringBuilder(1024);
        List<Area> m_areas = new List<Area>();
    }
}

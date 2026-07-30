using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace MoeConsole
{
    public class Location
    {
        public enum DIFFICULTY
        {
            VERY_EASY,
            EASY,
            NORMAL,
            HARD,
            IMPOSSIBLE,
            DIFFICULTY_MAX
        };

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("7D5339E3-2CA0-4EDB-8277-6140AC1F5F05")]
        protected interface ILOCATION
        {
            void AddNeighbor(IntPtr iNeighbor, int travelTimeSec);
            void AddEncounter([MarshalAs(UnmanagedType.LPWStr)] string strTag, int dif, int nMax);
            void AddPlaceable([MarshalAs(UnmanagedType.LPWStr)] string strName, [MarshalAs(UnmanagedType.LPWStr)] string szTag);
            void GetProperties(ref IntPtr iPrp);
        }

        public Location(IntPtr iunk)
        {
            IntPtr iPrp = IntPtr.Zero;

            m_iunk = iunk;
            m_iLocation = (ILOCATION)Marshal.GetObjectForIUnknown(m_iunk);            
        }

        public void Dispose()
        {
            m_properties.Dispose();

            if (m_iLocation != null)
            {
                Marshal.ReleaseComObject(m_iLocation);
                m_iLocation = null;
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
            }
        }

        public void AddNeighbor(Location loc, int travelTimeSec)
        {
            m_iLocation.AddNeighbor(loc.m_iunk, travelTimeSec);

        }

        public void AddEncounter(string strTag, DIFFICULTY dif, int nMax)
        {
            m_iLocation.AddEncounter(strTag, (int)dif, nMax);

        }

        public void AddPlaceable(string strName, string strPlaceable)
        {
            m_iLocation.AddPlaceable(strName, strPlaceable);
        }

        IntPtr m_iunk;
        ILOCATION m_iLocation;
        VariableCollection m_properties;
    }

    public class Creature
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("8346DA8D-9EFD-436A-9545-8D324DE0E43E")]
        protected interface ICREATURE
        {
            void Tick(int nSec);
	        void SetArea(IntPtr iArea);
        }

        public enum CLASS
        {
            CLASS_NONE,
            CLASS_WARRIOR,
            CLASS_MAGE,
            CLASS_ROGUE,
            CLASS_MAX
        }

        public void SetArea(Area area)
        {

        }
    }

    public class Character : Creature
    {
        public enum BACKGROUND
        {
            BACKGROUND_NONE,
            BACKGROUND_NOBLE,
            BACKGROUND_PEASANT,
            BACKGROUND_OUTLAW,
            BACKGROUND_MAX
        }

        public Character(IntPtr iunk)
        {
            m_iunk = iunk;
            m_iCreature = (ICREATURE)Marshal.GetObjectForIUnknown(m_iunk);
        }

        public void Dispose()
        {
            if (m_iCreature != null)
            {
                Marshal.ReleaseComObject(m_iCreature);
                m_iCreature = null;
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
            }
        }

        public void SetArea(Area ar)
        {
            m_iCreature.SetArea(ar.theUnknown);
        }

        IntPtr m_iunk = IntPtr.Zero;
        ICREATURE m_iCreature;
    }




    public class Quest
    {
        
        public Quest(IntPtr iunk)
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

        public class Area
    {
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("9E39F153-0811-498B-B72A-5C810D0141A0")]
        private interface IAREA
        {
            void AddLocation(ref IntPtr iLoc);
            void GetProperties(ref IntPtr iVarList);
            void AddQuest(ref IntPtr iQuest, [MarshalAs(UnmanagedType.LPWStr)] string strName, int nDuration, int nDifficulty);
            void GetLocation(int ndx, ref IntPtr iLoc);
            void SetOnEnterHandler([MarshalAs(UnmanagedType.LPWStr)] string szFnName);
            void UnitTest();
        }

        public Area(IntPtr iunk) 
        {
            IntPtr iVarList = IntPtr.Zero;

            m_iunk = iunk;
            m_iArea = (IAREA)Marshal.GetObjectForIUnknown(m_iunk);
            m_iArea.GetProperties(ref iVarList);

            m_properties = new VariableCollection();
        }

        public void Dispose()
        {
            m_properties.Dispose();
            foreach (Location l in m_locations)
                l.Dispose();

            if (m_iArea != null)
            {
                Marshal.ReleaseComObject(m_iArea);
                MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);
                m_iunk = IntPtr.Zero;
                m_iArea = null;
            }
        }

        public Location AddLocation()
        {
            IntPtr iunk = IntPtr.Zero;
            m_iArea.AddLocation(ref iunk);

            Location l = new Location(iunk);
            m_locations.Add(l);
            return l;
        }

        public Quest AddQuest(string strName, int nDuration, int nDifficult)
        {
            IntPtr iunk = IntPtr.Zero;
            m_iArea.AddQuest(ref iunk, strName, nDuration, nDifficult);
            return new Quest(iunk);

        }

        public Location GetLocation(int ndx)
        {
            if (ndx <= m_locations.Count)
                return m_locations[ndx];
            else
                return null;        
        }

        public void SetOnEnterHandler(string strFnName)
        {
            m_iArea.SetOnEnterHandler(strFnName);
        }

        public void UnitTest(Testing tst)
        {
            
        }

        public IntPtr theUnknown
        {
            get { return m_iunk; }
        }
        private IntPtr m_iunk = IntPtr.Zero;

        public string Name
        {
            get 
            {
                return m_name; 
            }
            set
            {
                m_name = value;                
            }
        }
        private string m_name = "";

       

        private IAREA m_iArea = null;
        private List<Location> m_locations = new List<Location>();
        private VariableCollection m_properties;
    }

    public class AstralWorkshop : IDisposable
    {
        static public uint classID = 133671;

        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("F43BA252-2FCA-41A2-9CFA-22A5D87C584C")]
        private interface IASTRALWORKSHOP
        {
            void NewArea(ref IntPtr iArea, ref int ndx, int nDanger);
            void Export([MarshalAs(UnmanagedType.LPWStr)] string strPath);
            void Save([MarshalAs(UnmanagedType.LPWStr)] string strPath);
            void NewCharacter(ref IntPtr iCharacter, [MarshalAs(UnmanagedType.LPWStr)] string szName, int nClass, int nBackground);
            void Heartbeat(int nSec);
            void GetTester(ref IntPtr iTst);
            void GetAreaNames([MarshalAs(UnmanagedType.BStr)] out string areaList);
            void UnitTest();
        }

        public AstralWorkshop()
        {
            MoeSzyslakLibrary.CreateMoeSzyslakInterface(classID, ref m_iunk);
            m_iAstralWorkshop = (IASTRALWORKSHOP)Marshal.GetObjectForIUnknown(m_iunk);
            IntPtr iunk = IntPtr.Zero;
            m_iAstralWorkshop.GetTester(ref iunk);
            m_test = new Testing(iunk);
        }

        ~AstralWorkshop() => Dispose(false);

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

            if (m_character != null)
                m_character.Dispose();

            m_test.Dispose();

            Marshal.ReleaseComObject(m_iAstralWorkshop);
            MoeSzyslakLibrary.FreeMoeSzyslakInterface(m_iunk);

            m_iAstralWorkshop = null;
            m_iunk = IntPtr.Zero;
        }

        public Area NewArea(string strName, int nDanger)
        {
            IntPtr iArea = IntPtr.Zero;
            int ndx = 0;

            m_iAstralWorkshop.NewArea(ref iArea, ref ndx, nDanger);
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

        public Character NewCharacter(string strName, Creature.CLASS nClass, Character.BACKGROUND bkgd)
        {
            if (m_character != null)
                m_character.Dispose();

            IntPtr iChrctr = IntPtr.Zero;
            m_iAstralWorkshop.NewCharacter(ref iChrctr, strName, (int)nClass, (int)bkgd);
            m_character = new Character(iChrctr);
            return m_character;
        }

        void Heartbeat(int nSec)
        {
            m_iAstralWorkshop.Heartbeat(nSec);
        }

        string[] GetAreaNames()
        {
            string nmes;
            m_iAstralWorkshop.GetAreaNames(out nmes);
            return nmes.Split('\n');

        }

        public static void UnitTest()
        {
            using (AstralWorkshop aw = new AstralWorkshop())
            {
                try
                {
                    aw.m_test.Message("Astral Workshop unit test", Testing.DEBUG_LEVEL.DEBUG_INFO, "Astral Workshop");
                    aw.NewArea("Nightworld of Vlad Tolenkov", 2);

                    Area a = aw.NewArea("The Misty Border", 2);

                    string[] nmes = aw.GetAreaNames();

                    Character c = aw.NewCharacter("Gert Addams", Creature.CLASS.CLASS_WARRIOR, Character.BACKGROUND.BACKGROUND_PEASANT);
                    c.SetArea(a);                   

                    a.UnitTest(aw.m_test);
                    aw.Heartbeat(60);

                    aw.Export("astralworkshop.xml");
                    aw.Save("astralworkshop.dat");

                    aw.m_iAstralWorkshop.UnitTest();

                    aw.m_test.Message("Test complete", Testing.DEBUG_LEVEL.DEBUG_INFO, "Astral Workshop");
                }
                catch (Exception e)
                {
                    aw.m_test.Verify(false, e.Message, (int)Testing.DEBUG_LEVEL.DEBUG_CRITICAL, "AstralWorkshop");
                }

                aw.m_test.Report();
            }
        }


        private IntPtr m_iunk = IntPtr.Zero;
        private IASTRALWORKSHOP m_iAstralWorkshop;
        private StringBuilder m_sb = new StringBuilder(1024);
        List<Area> m_areas = new List<Area>();
        Character m_character;
        Testing m_test;
    }
}

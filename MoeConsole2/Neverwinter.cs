using HtmlAgilityPack;
using System;
using System.Collections.Generic;
//using System;
using System.Net.Http;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Threading.Tasks;
using static MoeConsole.MoeSzyslakLibrary;


namespace MoeConsole
{
    public class Module
    {
        public Module(uint hNWNObj)
        {
            m_hObj = hNWNObj;

        }

        public void Commit()
        {
            
        }

        public void Print()
        {
            Console.WriteLine("Module Name:\t{0}", m_name);
        }

        private uint m_hObj;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        private string m_name = "New Module";
    }

    public class Neverwinter
    {
        public Neverwinter()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
            m_module = new Module(m_hObj);
        }

        ~Neverwinter()
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

        uint m_hObj;

        static public void UnitTest()
        {
            Testing tst = new Testing(IntPtr.Zero);
            tst.Dispose();

            /*Neverwinter nw = new Neverwinter();
            nw.m_module.Name = "Out of the Abyss.";
            nw.m_module.Commit();

            nw.m_module.Update();
            nw.m_module.Print();

            nw.Dispose();*/
        }

        public void Start()
        {
            string cmdline = "", prevCmd = "";

            while (!m_gameDown)
            {
                Console.Write("Enter command:  ");
                cmdline = Console.ReadLine();
                cmdline = cmdline.Trim();
                m_output = "";

                if (cmdline.Length == 0 && prevCmd.Length > 0)
                    cmdline = prevCmd;

                if (cmdline.Length > 0)
                    interpret(cmdline);

                Console.WriteLine(m_output);
            }

        }

        // The main entry point for executing commands.
        void interpret(string argument)
        {
            string[] wrds = argument.Split(' ');
            wrds[0] = wrds[0].ToLower();

            if (!m_testValues.ContainsKey(wrds[0]))
            {
                m_output = "Unknown command - " + wrds[0];
            }
            else
                m_testValues[wrds[0]](argument);
        }

        private void Exit(string param)
        {
            m_gameDown = true;
            m_output = "Shutting down.";
        }

        private void Edit(string param)
        {
            string cmdline;
            Console.Write("Module name ({0}):  ", m_module.Name);
            cmdline = Console.ReadLine();

            if (cmdline.Length > 0)
                m_module.Name = cmdline;

        }



        private const uint m_classID = 977093;
        private bool m_gameDown = false;
        private string m_output = "";
        private Module m_module;

        private delegate void DO_FUN(string param);
        private Dictionary<string, DO_FUN> m_testValues = new Dictionary<string, DO_FUN>();
    }

    public class TripPlanner : IDisposable
    {
        public TripPlanner()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
        }

        ~TripPlanner() => Dispose(false);

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            m_parameters.Dispose();

            if (m_hObj > 0)
            {
                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hObj);
                m_hObj = 0;
            }
        }

        
        static public void UnitTest()
        {
            using (TripPlanner trp = new TripPlanner())
            {
                trp.Name = "Waxahachie";
            }
        }        

        public string Name
        {
            get { return m_name; }
            set 
            {
                m_parameters.Set("funcID", 1);
                MoeSzyslakLibrary.InvokeMoeSzyslakHandle(m_hObj, m_parameters.m_hObj);
                m_name = value; 
            }
        }
        private string m_name = "New Trip";

        private const uint m_classID = 507734;
        private uint m_hObj;
        private VariableCollection m_parameters = new VariableCollection();
    }
}
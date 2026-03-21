using System;
using System.Collections.Generic;

namespace MoeConsole
{
    public class Neverwinter
    {
        public Neverwinter()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);

        }

        ~Neverwinter()
        {
            Dispose();
        }

        public void Dispose()
        {

            if (m_hObj > 0)
            {
                MoeSzyslakLibrary.InvokeHandle(m_hObj, "exit");
                Console.WriteLine(MoeSzyslakLibrary.GetReturnString(m_hObj));

                MoeSzyslakLibrary.DestroyMoeSzyslakHandle(m_hObj);
                m_hObj = 0;
            }
        }

        uint m_hObj;


        static public void UnitTest()
        {
            Testing tst = new Testing();
            tst.RunTest(m_classID);

            /*MoeSzyslakLibrary.InvokeHandle(m_hObj, "user Login");
            Console.WriteLine(MoeSzyslakLibrary.GetReturnString(m_hObj));*/

            tst.Dispose();
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
        private Module m_module = new Module();

        private delegate void DO_FUN(string param);
        private Dictionary<string, DO_FUN> m_testValues = new Dictionary<string, DO_FUN>();
    }
}
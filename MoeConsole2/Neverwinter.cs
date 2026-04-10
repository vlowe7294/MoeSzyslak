using HtmlAgilityPack;
using System;
using System.Collections.Generic;
//using System;
using System.Net.Http;
using System.Threading.Tasks;
using static MoeConsole.MoeSzyslakLibrary;


namespace MoeConsole
{
    public class GoogleDocParser
    {
        public static void ReadDoc()
        {
            string url = "https://docs.google.com/document/d/e/2PACX-1vTMOmshQe8YvaRXi6gEPKKlsC6UpFJSMAk4mQjLm_u1gmHdVVTaeh7nBNFBRlui0sTZ-snGwZM4DBCT/pub"; // published URL

            // Create HttpClient
            var client = new HttpClient();

            // Synchronous download
            string html = client.GetStringAsync(url).GetAwaiter().GetResult();

            // Parse HTML
            var doc = new HtmlDocument();
            doc.LoadHtml(html);

            // Extract text
            // string text = doc.DocumentNode.InnerText;

            string text = "0█00█10█21▀11▀22▀12▀23▀2";

            var points = new List<(char ch, int x, int y)>();

            for (int i = 0; i < text.Length; i += 3)
            {
                int x = int.Parse(text[i].ToString());
                char ch = text[i + 1];
                int y = int.Parse(text[i + 2].ToString());

                points.Add((ch, x, y));
            }

            Console.WriteLine(text);

        }
    }

    public class Module
    {
        public Module(uint hNWNObj)
        {
            m_hObj = hNWNObj;

        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("Module get \"Module Name\""));
            m_name = MoeSzyslakLibrary.GetReturnString(m_hObj);

        }

        public void Commit()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("Module set \"Module Name\" \"{0}\"", m_name));

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
            tst.Dispose();

            GoogleDocParser.ReadDoc();

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

    public class TripPlanner
    {
        public class Stop
        {
            
        }

        public TripPlanner()
        {
            m_hObj = MoeSzyslakLibrary.CreateMoeSzyslakHandle(m_classID);
        }

        ~TripPlanner()
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

        public void Commit()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("set \"Trip Name\" \"{0}\"", m_name));
        }

        public void Update()
        {
            MoeSzyslakLibrary.InvokeHandle(m_hObj, string.Format("get \"Trip Name\""));
            m_name = MoeSzyslakLibrary.GetReturnString(m_hObj);
        }

        public void AddStop()
        {
            m_stops.Add(new Stop());
            MoeSzyslakLibrary.InvokeHandle(m_hObj, "AddStop");
            string cnt  = MoeSzyslakLibrary.GetReturnString(m_hObj);
        }

        static public void UnitTest()
        {
            Testing tst = new Testing();
            tst.RunTest(m_classID);
            tst.Dispose();

            /*TripPlanner trp = new TripPlanner();
            trp.Name = "Chicago Trip";
            trp.AddStop();

            trp.Commit();
            trp.Update();
            trp.Dispose();*/
        }

        private const uint m_classID = 507734;
        private uint m_hObj;

        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }

        private string m_name = "New Trip";
        private List<Stop> m_stops = new List<Stop>();
    }
}
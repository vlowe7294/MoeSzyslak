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

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                MoeSzyslakLibrary.VerifyLibrary();
                TripPlanner.UnitTest();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
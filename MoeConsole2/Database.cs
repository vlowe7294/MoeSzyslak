using System;
using System.Data.SQLite;
using static System.Net.Mime.MediaTypeNames;

namespace MoeConsole
{
    public class Table
    {
        public Table(string nme) 
        {
            m_name = nme;
        }  

        public void Write(SQLiteConnection conn)
        {
            // Ensure table exists
            string sql = @"CREATE TABLE IF NOT EXISTS " + m_name + @" (username TEXT PRIMARY KEY, password TEXT)";

            using var cmd = new SQLiteCommand(sql, conn);
            cmd.ExecuteNonQuery();
        }

        private string m_name;

    }

    public class Database
    {
        public Database()
        {
            

            
        }

        public Table GetTable(string strNme)
        {
            Table tbl;

            if (m_tables.ContainsKey(strNme))
            {
                tbl = m_tables[strNme];
            }
            else
            {
                tbl = new Table(strNme);
                m_tables.Add(strNme, tbl);
            }

            return tbl;
        }

        public void Write(string strPath)
        {
            SQLiteConnection conn = new SQLiteConnection("Data Source=" + strPath + ";Version=3;"); 
            conn.Open();

            foreach (var (key, value) in m_tables)
            {
                value.Write(conn);
            }


        }

        Dictionary<string, Table> m_tables = new Dictionary<string, Table>();
    }
}
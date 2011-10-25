using System;
using System.Collections;
using System.IO;
using System.Management;
using Microsoft.Win32;

namespace InstalledSoftware
{
    class InstalledSoftware
    {
        private const int VERSION_MAJOR = 2;
        private const int VERSION_MINOR = 0;

        private const int ERROR_INVALID_PATH = -1;
        private const int ERROR_BAD_REGISTRY = -2;

        private const string SoftwareListKey1 = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
        private const string SoftwareListKey2 = "Software\\Classes\\Installer\\Products";
        private const string SoftwareListKey3 = "Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

        struct SoftwareEntry
        {
            public string InstallDate;
            public string DisplayName;
            public string DisplayVersion;
        }

        private static SortedList SoftwareList;
        private static StreamWriter file;

        private static bool WriteToFile;
        private static bool x64 = false;

        private static string ComputerName = "";
        private static string OutputPath = "";

        private static void QueryKey(RegistryKey key)
        {
            SoftwareEntry e = new SoftwareEntry();

            try
            {
                e.DisplayName = key.GetValue("DisplayName").ToString();
            }
            catch (Exception)
            {
                return;
            }

            try
            {
                e.InstallDate = key.GetValue("InstallDate").ToString();
            }
            catch (Exception)
            {
                e.InstallDate = "N/A";
            }

            try
            {
                e.DisplayVersion = key.GetValue("DisplayVersion").ToString();
            }
            catch (Exception)
            {
                e.DisplayVersion = "N/A";
            }

            try
            {
                if (!SoftwareList.ContainsKey(e.DisplayName))
                {
                    // If the software entry doesn't already exist, add it.
                    SoftwareList.Add(e.DisplayName, e);
                }
                else
                {
                    // If the software entry exists, merge the data with the
                    // existing entry.
                    SoftwareEntry e2 = (SoftwareEntry)SoftwareList[e.DisplayName];

                    if (e2.InstallDate == "N/A") e2.InstallDate = e.InstallDate;
                    if (e2.DisplayVersion == "N/A") e2.DisplayVersion = e.DisplayVersion;
                }
            }
            catch (Exception)
            {
            }
        }

        private static void DisplaySoftwareList()
        {
            if (WriteToFile)
            {
                file.WriteLine("Computer Name: " + ComputerName);

                if (OutputPath.Length > 0) file.WriteLine("Output Path:   " + OutputPath);

                if (x64)
                {
                    file.WriteLine("Architecture:  64-bit");
                }
                else
                {
                    file.WriteLine("Architecture:  32-bit");
                }

                file.WriteLine("----------------------------------------------------");
                file.WriteLine();
                file.WriteLine("{0,-20}Program Name", "Install Date");
                file.WriteLine();

                for (int i = 0; i < SoftwareList.Count; i++)
                {
                    SoftwareEntry e = (SoftwareEntry)SoftwareList.GetByIndex(i);

                    file.WriteLine("{0,-20}{1} -- {2}", e.InstallDate, e.DisplayName, e.DisplayVersion);
                }
            }
            else
            {
                Console.WriteLine("Computer Name: " + ComputerName);

                if (OutputPath.Length > 0) Console.WriteLine("Output Path:   " + OutputPath);

                if (x64)
                {
                    Console.WriteLine("Architecture:  64-bit");
                }
                else
                {
                    Console.WriteLine("Architecture:  32-bit");
                }

                Console.WriteLine("----------------------------------------------------");
                Console.WriteLine();
                Console.WriteLine("{0,-20}Program Name", "Install Date");
                Console.WriteLine();

                for (int i = 0; i < SoftwareList.Count; i++)
                {
                    SoftwareEntry e = (SoftwareEntry)SoftwareList.GetByIndex(i);

                    Console.WriteLine("{0,-20}{1} -- {2}", e.InstallDate, e.DisplayName, e.DisplayVersion);
                }
            }
        }

        // Returns false if 32-bit, true if 64-bit.
        private static bool GetOsArchitecture(string ComputerName)
        {
            // Querying WMI for the architecture of a remote Windows computer
            // seems to be the only reliable way to do this. Unfortunately,
            // only Vista and higher contain the OSArchitecture field for the
            // class Win32_OperatingSystem. If this is run against an XP box,
            // for now you are SoL until a better way is found.
            try
            {
                ManagementScope scope = new ManagementScope("\\\\" + ComputerName + "\\root\\CIMV2");
                WqlObjectQuery query = new WqlObjectQuery("SELECT * FROM Win32_OperatingSystem");
                ManagementObjectSearcher searcher = new ManagementObjectSearcher(scope, query);

                foreach (ManagementObject os in searcher.Get())
                {
                    if (os["OSArchitecture"].ToString().Contains("64"))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            catch (Exception)
            {
                // This will thrown an exception most likely in the cases of
                // the computer being offline or OSArchitecture not being
                // present (as is the case in pre-Vista).
            }

            return false;
        }

        public static int Main(string[] argv)
        {
            RegistryKey HKLM;
            RegistryKey Key;

            WriteToFile = false;

            // Parse the command line parameters.
            if (argv.Length > 0)
            {
                if (argv[0] == "/?")
                {
                    // Display the help.
                    Console.WriteLine("instsoft version " + VERSION_MAJOR + "." + VERSION_MINOR + ", Copyright (c) 2011, Lucas M. Suggs");
                    Console.WriteLine("Usage: instsoft.exe [/f path] [computername]");

                    return 0;
                }

                if ((argv[0] == "/f") && (argv.Length > 1))
                {
                    // User specified to output to a path rather than stdout.
                    OutputPath = argv[1] + "\\";

                    WriteToFile = true;

                    if (!Directory.Exists(OutputPath))
                    {
                        Console.Error.WriteLine("Invalid output path: " + OutputPath);

                        return ERROR_INVALID_PATH;
                    }

                    if (argv.Length > 2)
                    {
                        ComputerName = argv[2];
                    }
                    else
                    {
                        ComputerName = Environment.MachineName;
                    }
                }
                else
                {
                    // We don't recognize the first parameter as a flag, so it
                    // is assumed that it is a computer name.
                    ComputerName = argv[0];
                }
            }
            else
            {
                // No command line parameters, so grab the local computer name.
                ComputerName = Environment.MachineName;
            }

            // Figure out the OS architecture of the computer name we are using.
            x64 = GetOsArchitecture(ComputerName);

            if (WriteToFile)
            {
                // Since we are writing to a file, craft the file name.
                string FileName = OutputPath + ComputerName + "_";

                // DateTime.Now.ToString("s") outputs to the format:
                //    2011-12-08T16:01:35
                // So we want to strip the - and : and turn the T into a - so 
                // we get:
                //    20111208-160135
                FileName += DateTime.Now.ToString("s").Replace(":", "").Replace("-", "").Replace("T", "-") + ".txt";

                file = File.CreateText(FileName);

                Console.WriteLine(FileName);
            }

            try
            {
                // Attempt to open the registry of the remote computer.
                HKLM = RegistryKey.OpenRemoteBaseKey(RegistryHive.LocalMachine, ComputerName);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Unable to open the registry:");
                Console.Error.WriteLine(e);

                return ERROR_BAD_REGISTRY;
            }

            // Set up the software list.
            SoftwareList = new SortedList();

            try
            {
                // This first registry location is the MSI uninstall records.
                // This location contains the most detailed information, but 
                // also does not contain a list of all installed software.
                Key = HKLM.OpenSubKey(SoftwareListKey1);

                foreach (string key in Key.GetSubKeyNames())
                {
                    RegistryKey SubKey = Key.OpenSubKey(key);

                    QueryKey(SubKey);
                }

                // The second registry location is the MSI install records;
                // however, this location, while having entries that are missing
                // in the MSI uninstall records, does not have install date or
                // program version information for installed software (WTF?).
                Key = HKLM.OpenSubKey(SoftwareListKey2);

                foreach (string key in Key.GetSubKeyNames())
                {
                    RegistryKey SubKey = Key.OpenSubKey(key);

                    QueryKey(SubKey);
                }

                if (x64)
                {
                    // The third registry location is the WoW6432node MSI
                    // uninstall records. This will pull software entries
                    // that were installed with an MSI flagged as 64-bit.
                    //
                    // Due to registry virtualization, this seems to be the only
                    // way to get the opposite architecture's list of installed
                    // software (opposite of the architecture of the currently
                    // running instsoft program).
                    Key = HKLM.OpenSubKey(SoftwareListKey3);

                    foreach (string key in Key.GetSubKeyNames())
                    {
                        RegistryKey SubKey = Key.OpenSubKey(key);

                        QueryKey(SubKey);
                    }
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e);
            }

            // Display the software list.
            DisplaySoftwareList();

            return 0;
        }
    }
}

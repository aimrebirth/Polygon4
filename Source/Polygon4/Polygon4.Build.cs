/*
 * Polygon-4
 * Copyright (C) 2015 lzwdgc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

using System;
using System.IO;
using System.Diagnostics;

using UnrealBuildTool;

public class Polygon4 : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

    //private static int NumberOfCalls = 0;

    public Polygon4(ReadOnlyTargetRules Target)
        : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;
        PrivatePCHHeaderFile = "Polygon4.h";
        CppStandard = CppStandardVersion.Cpp17;
        bEnforceIWYU = true;

        PublicDependencyModuleNames .AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Landscape", "AIModule" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        GenerateVersion();
        LoadCoreModule(Target, "Engine");
    }

    void GenerateVersion()
    {
        bool has_git_dir = Directory.Exists(Path.Combine(ModuleDirectory, "../../.git/"));

        Process process = new Process();
        process.StartInfo.FileName = "git";
        process.StartInfo.Arguments = "--version";
        process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
        process.StartInfo.WorkingDirectory = ModuleDirectory;
        process.Start();
        process.WaitForExit();
        bool has_git = process.ExitCode == 0;

        var dir = Path.Combine(ModuleDirectory, "Generated");
        Directory.CreateDirectory(dir);

        string version = "";
        bool written = false;
        if (has_git_dir && has_git)
        {
            string stdout = "";

            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.UseShellExecute = false;

            process.StartInfo.Arguments = "rev-parse --short HEAD";
            process.Start();
            process.WaitForExit();
            written |= process.ExitCode == 0;

            stdout = process.StandardOutput.ReadToEnd();
            stdout = stdout.Substring(0, stdout.Length - 1);
            version += stdout;

            process.StartInfo.Arguments = "rev-parse --abbrev-ref HEAD";
            process.Start();
            process.WaitForExit();
            written |= process.ExitCode == 0;

            stdout = process.StandardOutput.ReadToEnd();
            stdout = stdout.Substring(0, stdout.Length - 1);
            version += "+" + stdout;
        }
        version = "\"" + version + "\"";
        var p = Path.Combine(ModuleDirectory, "Generated/Version.h");
        if (File.Exists(p))
        {
            var t = File.ReadAllText(p);
            if (t != version)
                File.WriteAllText(p, version);
        }
        else
            File.WriteAllText(p, version);
    }

    bool RemoveLibrary(string dst)
    {
        bool removed = true;
        string dll = dst + ".dll";
        string pdb = dst + ".pdb";
        if (File.Exists(dll))
        {
            try
            {
                string msg = "Deleting old hot reload file: \"{0}\".";
                File.Delete(dll);
                System.Console.WriteLine(msg, dll);
                File.Delete(pdb);
                System.Console.WriteLine(msg, pdb);
            }
            catch (System.Exception)
            {
                removed = false;
            }
        }
        return removed;
    }

    bool RemoveLogs(string dst)
    {
        bool removed = true;

        string log_info = dst + ".log.info";
        try
        {
            File.Delete(log_info);
        }
        catch (System.Exception)
        {
            removed = false;
        }

        string log_debug = dst + ".log.debug";
        try
        {
            File.Delete(log_debug);
        }
        catch (System.Exception)
        {
            removed = false;
        }

        string log_trace = dst + ".log.trace";
        try
        {
            File.Delete(log_trace);
        }
        catch (System.Exception)
        {
            removed = false;
        }

        return removed;
    }

    public void LoadCoreModule(ReadOnlyTargetRules Target, string Name)
    {
        //System.Console.WriteLine("Enter LoadCoreModule");

        if (Target.Platform != UnrealTargetPlatform.Win64 && Target.Platform != UnrealTargetPlatform.Win32)
            return;

        //PublicDefinitions.Add("SCHEMA_API=");
        //PublicDefinitions.Add("DATA_MANAGER_API=");
        //PublicDefinitions.Add("SCHEMA_API=__declspec(dllimport)");
        //PublicDefinitions.Add("DATA_MANAGER_API=__declspec(dllimport)");
        PublicDefinitions.Add("P4_ENGINE_API=__declspec(dllimport)");

        // idirs, libs
        {
            var defs = File.ReadLines(Path.Combine(ThirdPartyPath, Name, ".sw", "definitions_ReleaseWithDebugInformation.txt"));
            foreach (var s in defs)
                if (!s.StartsWith("P4_ENGINE_API="))
                    PublicDefinitions.Add(s);

            var includes = File.ReadLines(Path.Combine(ThirdPartyPath, Name, ".sw", "includes_ReleaseWithDebugInformation.txt"));
            foreach (var s in includes)
                PublicIncludePaths.Add(s);

            //foreach (var s in PublicIncludePaths)
            //    Console.WriteLine(s);

            /*var data_manager = File.ReadAllText(Path.Combine(ThirdPartyPath, Name, ".sw", "data_manager_ReleaseWithDebugInformation.txt"));
            var schema = File.ReadAllText(Path.Combine(ThirdPartyPath, Name, ".sw", "schema_ReleaseWithDebugInformation.txt"));
            PublicAdditionalLibraries.Add(data_manager);
            PublicAdditionalLibraries.Add(schema);*/

            var ll = File.ReadLines(Path.Combine(ThirdPartyPath, Name, ".sw", "link_libraries_ReleaseWithDebugInformation.txt"));
            foreach (var s in ll)
                PublicAdditionalLibraries.Add(s);

            //foreach (var s in PublicDelayLoadDLLs)
            //    Console.WriteLine(s);
        }

        var ell = File.ReadLines(Path.Combine(ThirdPartyPath, Name, ".sw", "engine_ReleaseWithDebugInformation.txt"));

        //string base_name = "Polygon4.Engine-master";
        //int base_name_id = 0;

        foreach (var s in ell)
            PublicAdditionalLibraries.Add(s);
        //PublicDelayLoadDLLs.Add(base_name + ".dll");

        /*string dst_base_name = Path.Combine(ModuleDirectory, "../../Binaries/", Target.Platform.ToString()) + "/" + base_name;
        dst_base_name = Path.GetFullPath(dst_base_name);

        string src = Path.Combine(BaseDir, base_name);
        string dst = dst_base_name;

        //if (Target.Type != TargetRules.TargetType.Editor)
        //    return;

        if (NumberOfCalls++ > 0)
            return;

        // try to remove previous dll, pdb
        base_name_id = 1;
        while (true)
        {
            dst = dst_base_name;
            if (base_name_id > 0)
                dst = dst_base_name + "." + base_name_id.ToString();
            if (!RemoveLibrary(dst))
                break;
            if (base_name_id++ > 1000)
                break;
        }
        // try to remove previous logs
        base_name_id = 0;
        while (true)
        {
            dst = dst_base_name;
            if (base_name_id > 0)
                dst = dst_base_name + "." + base_name_id.ToString();
            if (!RemoveLogs(dst))
                break;
            if (base_name_id++ > 1000)
                break;
        }*/
    }
}

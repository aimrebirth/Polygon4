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

    private static int NumberOfCalls = 0;

	public Polygon4(TargetInfo Target)
	{
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
        File.WriteAllText(Path.Combine(ModuleDirectory, "Generated/Version.h"), version);
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

    public void LoadCoreModule(TargetInfo Target, string Name)
    {
        //System.Console.WriteLine("Enter LoadCoreModule");

        if (Target.Platform != UnrealTargetPlatform.Win64 && Target.Platform != UnrealTargetPlatform.Win32)
            return;

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, Name, "include"));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "DataManager/include"));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "../../DataManager/include"));
        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
        {
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Engine/win64/gen_RelWithDebInfo/include"));
            }
            if (Target.Platform == UnrealTargetPlatform.Win32)
            {
                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Engine/win32/gen_RelWithDebInfo/include"));
            }
        }

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        string BaseDir = Path.Combine(ThirdPartyPath, Name, "lib");
        BaseDir = Path.GetFullPath(BaseDir);

        string base_name = Name + "." + PlatformString;
        int base_name_id = 0;

        PublicAdditionalLibraries.Add(BaseDir + "/" + base_name + ".lib");
        PublicDelayLoadDLLs.Add(base_name + ".dll");

        string dst_base_name = Path.Combine(ModuleDirectory, "../../Binaries/", Target.Platform.ToString()) + "/" + base_name;
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
        }
    }
}

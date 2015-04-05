// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using System.Diagnostics;

using UnrealBuildTool;

public class Polygon4 : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    private static int NumberOfCalls = 0;

	public Polygon4(TargetInfo Target)
	{
		PublicDependencyModuleNames .AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        LoadCoreModule(Target, "Engine");
	}

    void MakeDir(string dst)
    {
        try
        {
            System.IO.Directory.CreateDirectory(Path.GetDirectoryName(dst));
        }
        catch (System.Exception)
        {
        }
    }

    bool CopyLibrary(string src, string dst, bool overwrite)
    {
        bool copied = true;
        string[] ext = { ".dll", ".pdb" };
        MakeDir(dst);
        try
        {
            foreach (var e in ext)
            {
                File.Copy(src + e, dst + e, overwrite);
            }
        }
        catch (System.Exception)
        {
            copied = false;
        }
        return copied;
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
        string log_debug = dst + ".log.debug";
        string log_trace = dst + ".log.trace";
        try
        {
            File.Delete(log_debug);
        }
        catch (System.Exception)
        {
            removed = false;
        }
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
        if (Target.Platform != UnrealTargetPlatform.Win64 && Target.Platform != UnrealTargetPlatform.Win32)
            return;

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, Name, "include"));

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        string BaseDir = Path.Combine(ThirdPartyPath, Name, "lib");
        BaseDir = Path.GetFullPath(BaseDir);

        string base_name = Name + "." + PlatformString;
        int base_name_id = 0;

        PublicAdditionalLibraries.Add(BaseDir + "/" + base_name + ".lib");

        string dst_base_name = Path.Combine(ModulePath, "../../Binaries/", Target.Platform.ToString()) + "/" + base_name;
        dst_base_name = Path.GetFullPath(dst_base_name);

        string src = Path.Combine(BaseDir, base_name);
        string dst = dst_base_name;

        if (Target.Type != TargetRules.TargetType.Editor)
            return;

        if (NumberOfCalls++ > 0)
            return;

        bool copied = CopyLibrary(src, dst, true);

        if (copied)
        {
            // try remove previous dll, pdb
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
            // try remove previous logs
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
}

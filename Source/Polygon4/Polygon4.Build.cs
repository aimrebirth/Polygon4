// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;

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

	public Polygon4(TargetInfo Target)
	{
		PublicDependencyModuleNames .AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        LoadCoreModule(Target, "Engine");
	}

    public bool LoadCoreModule(TargetInfo Target, string Name)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, Name, "lib");

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, Name + "." + PlatformString + ".lib"));

            string src = Path.Combine(LibrariesPath, Name + "." + PlatformString + ".dll");
            string dst = Path.Combine(ModulePath, "../../Binaries/", Target.Platform.ToString()) + "/" + Path.GetFileName(src);

            try { File.Copy(src, dst, true); } catch (System.Exception) {}

            /*System.Console.WriteLine(Target.Platform);
            System.Console.WriteLine(Target.Configuration);
            System.Console.WriteLine(Target.Type);*/
        }

        if (isLibrarySupported)
        {
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, Name, "include"));
        }

        return isLibrarySupported;
    }
}

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

public class DBTool : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
        //get { return Path.GetDirectoryName(RulesCompiler.GetFileNameFromType(this.GetType())); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../../../ThirdParty/")); }
    }

    public DBTool(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...
                "Landscape",
                "Polygon4",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
            }
			);

        LoadCoreModule(Target, "Engine");
    }

    public void LoadCoreModule(TargetInfo Target, string Name)
    {
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, Name, "include"));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "DataManager/include"));
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "../../DataManager/include"));

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        string BaseDir = Path.Combine(ThirdPartyPath, Name, "lib");
        BaseDir = Path.GetFullPath(BaseDir);

        string base_name = Name + "." + PlatformString;

        PublicAdditionalLibraries.Add(BaseDir + "/" + base_name + ".lib");
    }
}

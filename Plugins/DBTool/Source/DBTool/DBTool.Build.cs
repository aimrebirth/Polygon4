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
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../ThirdParty/")); }
    }

    public DBTool(ReadOnlyTargetRules Target)
        : base(Target)
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
                //
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

    public void LoadCoreModule(ReadOnlyTargetRules Target, string Name)
    {
        PublicDefinitions.Add("SCHEMA_API=__declspec(dllimport)");
        PublicDefinitions.Add("DATA_MANAGER_API=__declspec(dllimport)");
        PublicDefinitions.Add("P4_ENGINE_API=__declspec(dllimport)");

        // idirs, libs
        {
            var includes_file = File.ReadAllText(Path.Combine(ThirdPartyPath, Name, "win64", "includes.txt"));
            var includes = includes_file.Split(';');
            foreach (var s in includes)
                PublicIncludePaths.Add(s);
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, Name, "include"));

            //foreach (var s in PublicIncludePaths)
            //    Console.WriteLine(s);

            var data_manager = File.ReadAllText(Path.Combine(ThirdPartyPath, Name, "win64", "data_manager_RelWithDebInfo.txt"));
            var schema = File.ReadAllText(Path.Combine(ThirdPartyPath, Name, "win64", "schema_RelWithDebInfo.txt"));
            PublicAdditionalLibraries.Add(data_manager);
            PublicAdditionalLibraries.Add(schema);

            //PublicDelayLoadDLLs.Add(data_manager.Replace(".lib", ".dll").Replace("/lib", "/bin"));
            //PublicDelayLoadDLLs.Add(schema.Replace(".lib", ".dll").Replace("/lib", "/bin"));
        }

        string BaseDir = Path.Combine(ThirdPartyPath, Name, "win64", "bin", "RelWithDebInfo");
        BaseDir = Path.GetFullPath(BaseDir);
        string base_name = Name;

        PublicAdditionalLibraries.Add(BaseDir + "/" + base_name + ".lib");
        //PublicDelayLoadDLLs.Add(base_name + ".dll");
    }
}

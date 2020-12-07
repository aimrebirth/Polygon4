/*
 * Polygon-4
 * Copyright (C) 2020 Yakim3396
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

public class tools : ModuleRules
{
    public tools(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

		// Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "inc"));

        // Add any import libraries or static libraries
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Polygon4.Tools.common-master.lib"));
    }
}

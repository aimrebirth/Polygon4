/*
 * Polygon-4
 * Copyright (C) 2015-2020 lzwdgc
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

    public Polygon4(ReadOnlyTargetRules Target)
        : base(Target)
    {
        PrivatePCHHeaderFile = "Polygon4.h";
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bUseUnity = false;
        CppStandard = CppStandardVersion.Latest;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Landscape", "AIModule" });
        PrivateDependencyModuleNames.AddRange(new string[] { "PhysicsCore", "Slate", "SlateCore" });

        GenerateVersion();
        LoadEngine(Target);
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

    public void LoadEngine(ReadOnlyTargetRules Target)
    {
        var BuildName = "rwdi";

        var EnginePath = Path.Combine(ThirdPartyPath, "Engine");
        var DotSwPath = Path.Combine(EnginePath, ".sw");
        var JsonProgPath = Path.Combine(DotSwPath, "out", BuildName, "");
        var JsonPath = Path.Combine(DotSwPath, "g", "swbdesc", BuildName + ".json");

        var DefsFile = Path.Combine(DotSwPath, "defs.txt");
        var IdirsFile = Path.Combine(DotSwPath, "idirs.txt");
        var LibsFile = Path.Combine(DotSwPath, "libs.txt");

        if (!File.Exists(DefsFile) ||
            !File.Exists(IdirsFile) ||
            !File.Exists(LibsFile))
        {
            if (!RunSwCommand("sw", "get storage-dir", EnginePath))
                throw new Exception("Engine build failed: cannot run sw");
            if (stdout.Length == 0)
                throw new Exception("Engine build failed: empty storage dir");
            bool r =
                RunSwCommand("sw", "-static -config rwdi build", EnginePath) &&
                RunSwCommand("sw", "-static -config rwdi -build-name " + BuildName + " generate -g swbdesc", EnginePath) &&
                RunSwCommand("sw", "run Polygon4.Engine.tools.prepare_sw_info-0.0.1" +
                    " \"" + DotSwPath + "\"" +
                    " \"" + JsonPath + "\"" +
                    " \"" + stdout + "\"" +
                    " " + "Polygon4.Engine-master"
                    , EnginePath)
            ;
            if (!r)
                throw new Exception("Engine build failed");
        }

        PublicIncludePaths.Add(ModuleDirectory);

        // read & set defs, idirs, libs
        foreach (var s in File.ReadLines(DefsFile))
            PublicDefinitions.Add(s);

        foreach (var s in File.ReadLines(IdirsFile))
            PublicIncludePaths.Add(s);

        foreach (var s in File.ReadLines(LibsFile))
            PublicSystemLibraries.Add(s);
    }

    bool RunSwCommand(string Program, string Args, string Wdir)
    {
        try
        {
            return RunSwCommand1(Program, Args, Wdir);
        }
        catch (System.Exception)
        {
            return RunSwCommand1(Path.Combine(ModuleDirectory, "../../../BootstrapPrograms", Program), Args, Wdir);
        }
    }

    bool RunSwCommand1(string Program, string Args, string Wdir)
    {
        Console.WriteLine("Executing: " + Program + " " + Args);

        Process process = new Process();
        process.StartInfo.FileName = Program;
        process.StartInfo.Arguments = Args;
        process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
        process.StartInfo.WorkingDirectory = Wdir;
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.RedirectStandardOutput = true;
        process.StartInfo.RedirectStandardError = true;
        //process.OutputDataReceived += process_StdoutDataReceived;
        process.OutputDataReceived += (sender, args) => Console.WriteLine("sw: {0}", args.Data);
        process.ErrorDataReceived += process_DataReceived;
        process.ErrorDataReceived += (sender, args) => Console.WriteLine("sw: {0}", args.Data);
        process.Start();
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();
        process.WaitForExit();
        return process.ExitCode == 0;
    }

    string stdout;
    void process_DataReceived(object sender, DataReceivedEventArgs e)
    {
        stdout += e.Data;
    }
}

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
using System.Collections.Generic;

using UnrealBuildTool;

public class SwPackageLoader
{
    string p4_project_root;
    string engine_path;
    string storage_dir;
    string config_name = "rwdi";
    string stdout;
    List<string> defs = new List<string>();
    List<string> idirs = new List<string>();
    List<string> libs = new List<string>();

    // load package
    public SwPackageLoader(string p4_root)
    {
        p4_project_root = p4_root;
        engine_path = Path.Combine(p4_project_root, "ThirdParty/Engine");
    }

    public void LoadPackage(string package)
    {
        //var BuildName = package.GetHashCode().ToString();
        var BuildName = package;//.GetHashCode().ToString();
        Func<bool> f = delegate ()
        {
            return
                RunSwCommand("sw", "-static -config " + config_name + " build " + package, engine_path) &&
                RunSwCommand("sw", "-static -config " + config_name + " -build-name " + BuildName + " generate -g swbdesc " + package, engine_path) &&
                RunSwCommand("sw", "run Polygon4.Engine.tools.prepare_sw_info-0.0.1" +
                    " \"" + GetUe4SwPath(BuildName) + "\"" +
                    " \"" + GetJsonPath(BuildName) + "\"" +
                    " \"" + GetStorageDir() + "\"" +
                    " " + package
                    , engine_path)
                ;
        };
        Load(BuildName, f);
    }

    public void LoadDirectory(string target_dir, string main_target, string buildname)
    {
        //var BuildName = (target_dir + main_target).GetHashCode().ToString();
        var BuildName = buildname;
        Func<bool> f = delegate ()
        {
            return
                RunSwCommand("sw", "-static -config " + config_name + " build", target_dir) &&
                RunSwCommand("sw", "-static -config " + config_name + " -build-name " + BuildName + " generate -g swbdesc", target_dir) &&
                RunSwCommand("sw", "run Polygon4.Engine.tools.prepare_sw_info-0.0.1" +
                    " \"" + GetUe4SwPath(BuildName) + "\"" +
                    " \"" + GetJsonPath(BuildName) + "\"" +
                    " \"" + GetStorageDir() + "\"" +
                    " " + main_target
                    , engine_path)
                ;
        };
        Load(BuildName, f);
    }

    public void AddBuildSettings(ModuleRules r)
    {
        foreach (var s in defs)
            r.PublicDefinitions.Add(s);
        foreach (var s in idirs)
            r.PublicIncludePaths.Add(s);
        foreach (var s in libs)
            r.PublicSystemLibraries.Add(s);
    }

    void Load(string BuildName, Func<bool> build_function)
    {
        var p = GetUe4SwPath(BuildName);
        var DefsFile = Path.Combine(p, "defs.txt");
        var IdirsFile = Path.Combine(p, "idirs.txt");
        var LibsFile = Path.Combine(p, "libs.txt");

        if (!File.Exists(DefsFile)  ||
            !File.Exists(IdirsFile) ||
            !File.Exists(LibsFile))
        {
            if (!build_function())
                throw new Exception("Include sw package failed");
        }

        // read defs, idirs, libs
        foreach (var s in File.ReadLines(DefsFile))
            defs.Add(s);
        foreach (var s in File.ReadLines(IdirsFile))
            idirs.Add(s);
        foreach (var s in File.ReadLines(LibsFile))
            libs.Add(s);
    }

    string GetStorageDir()
    {
        if (storage_dir != null)
            return storage_dir;

        // set storage dir
        if (!RunSwCommand("sw", "get storage-dir", engine_path))
            throw new Exception("Engine build failed: cannot run sw");

        storage_dir = stdout;
        if (storage_dir.Length == 0)
            throw new Exception("Engine build failed: empty storage dir");

        return storage_dir;
    }

    string GetUe4SwPath(string BuildName)
    {
        return Path.GetFullPath(Path.Combine(p4_project_root, "Intermediate/sw", BuildName));
    }

    string GetJsonPath(string BuildName)
    {
        return Path.Combine(engine_path, ".sw", "g", "swbdesc", BuildName + ".json");
    }

    bool RunSwCommand(string Program, string Args, string Wdir)
    {
        try
        {
            return RunSwCommand1(Program, Args, Wdir);
        }
        catch (System.Exception)
        {
            return RunSwCommand1(Path.Combine(p4_project_root, "../BootstrapPrograms", Program), Args, Wdir);
        }
    }

    bool RunSwCommand1(string Program, string Args, string Wdir)
    {
        Console.WriteLine("Executing: " + Program + " " + Args);

        stdout = "";
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

    void process_DataReceived(object sender, DataReceivedEventArgs e)
    {
        stdout += e.Data;
    }
}

public class Polygon4 : ModuleRules
{
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

    void LoadEngine(ReadOnlyTargetRules Target)
    {
        PublicIncludePaths.Add(ModuleDirectory);

        // sw pkgs
        var P4Root = Path.GetFullPath(Path.Combine(ModuleDirectory, "../.."));
        var loader = new SwPackageLoader(P4Root);
        loader.LoadDirectory(Path.Combine(P4Root, "ThirdParty/Engine"), "Polygon4.Engine-master", "p4engine");
        loader.AddBuildSettings(this);
    }
}

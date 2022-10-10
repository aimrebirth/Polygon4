using UnrealBuildTool;
using System.Collections.Generic;

public class Polygon4EditorTarget : TargetRules
{
	public Polygon4EditorTarget(TargetInfo Target)
        : base(Target)
    {
        Type = TargetType.Editor;

        ExtraModuleNames.Add("Polygon4");

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}

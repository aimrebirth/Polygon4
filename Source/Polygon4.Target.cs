using UnrealBuildTool;
using System.Collections.Generic;

public class Polygon4Target : TargetRules
{
	public Polygon4Target(TargetInfo Target)
        : base(Target)
	{
        Type = TargetType.Game;

        ExtraModuleNames.Add("Polygon4");
    }
}

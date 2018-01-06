

using UnrealBuildTool;
using System.Collections.Generic;

public class TickTickBoomTarget : TargetRules
{
	public TickTickBoomTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "TickTickBoom" } );
	}
}



using UnrealBuildTool;
using System.Collections.Generic;

public class TickTickBoomEditorTarget : TargetRules
{
	public TickTickBoomEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "TickTickBoom" } );
	}
}

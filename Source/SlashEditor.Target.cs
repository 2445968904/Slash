

using UnrealBuildTool;
using System.Collections.Generic;

public class SlashEditorTarget : TargetRules
{
	public SlashEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Slash" } );
	}
}

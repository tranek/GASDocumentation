// Copyright 2019 Dan Kestranek.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDocumentationEditorTarget : TargetRules
{
	public GASDocumentationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("GASDocumentation");
	}
}

// Copyright 2020 Dan Kestranek.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDocumentationEditorTarget : TargetRules
{
	public GASDocumentationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GASDocumentation");
	}
}

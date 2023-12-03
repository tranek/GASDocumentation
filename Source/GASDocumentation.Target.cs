// Copyright 2020 Dan Kestranek.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDocumentationTarget : TargetRules
{
	public GASDocumentationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("GASDocumentation");
	}
}

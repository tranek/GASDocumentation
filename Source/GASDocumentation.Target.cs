// Copyright 2019 Dan Kestranek.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDocumentationTarget : TargetRules
{
	public GASDocumentationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("GASDocumentation");
	}
}

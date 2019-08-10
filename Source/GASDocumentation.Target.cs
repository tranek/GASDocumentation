// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

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

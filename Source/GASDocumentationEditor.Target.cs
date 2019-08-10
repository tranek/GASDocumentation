// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASDocumentationEditorTarget : TargetRules
{
	public GASDocumentationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("GASDocumentation");
	}
}

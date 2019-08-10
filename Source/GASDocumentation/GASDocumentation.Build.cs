// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASDocumentation : ModuleRules
{
	public GASDocumentation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });
    }
}

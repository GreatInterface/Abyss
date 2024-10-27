// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Abyss : ModuleRules
{
	public Abyss(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"ModularGameplay", 
			"GameplayAbilities", 
			"GameplayTags",
			"GameplayTasks",
			"GameFeatures",
			"ModularGameplayActors",
			"CommonGame",
			"CommonUser",
			"UMG",
			"NetCore"
			});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PublicDefinitions.Add("ENABLE_DRAW_DEBUG=1");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PrototipoShooter : ModuleRules
{
	public PrototipoShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"PrototipoShooter",
			"PrototipoShooter/Variant_Platforming",
			"PrototipoShooter/Variant_Platforming/Animation",
			"PrototipoShooter/Variant_Combat",
			"PrototipoShooter/Variant_Combat/AI",
			"PrototipoShooter/Variant_Combat/Animation",
			"PrototipoShooter/Variant_Combat/Gameplay",
			"PrototipoShooter/Variant_Combat/Interfaces",
			"PrototipoShooter/Variant_Combat/UI",
			"PrototipoShooter/Variant_SideScrolling",
			"PrototipoShooter/Variant_SideScrolling/AI",
			"PrototipoShooter/Variant_SideScrolling/Gameplay",
			"PrototipoShooter/Variant_SideScrolling/Interfaces",
			"PrototipoShooter/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

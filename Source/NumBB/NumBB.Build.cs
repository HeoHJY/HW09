// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NumBB : ModuleRules
{
	public NumBB(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", 
			
			//UI
			"UMG", "Slate", "SlateCore", 
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	
		PublicIncludePaths.AddRange(new string[] { "NumBB" });
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

// Copyright (c) 2026 GregOrigin. All Rights Reserved.

using UnrealBuildTool;

public class PSOAutopilotCore : ModuleRules
{
	public PSOAutopilotCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"DeveloperSettings",
				"AssetRegistry",
				"RenderCore",
				"UMG"
			}
		);
	}
}

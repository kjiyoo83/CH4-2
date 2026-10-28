// 저작권은 Epic Games, Inc.에 있으며 모든 권리를 보유함

using UnrealBuildTool;

public class CX : ModuleRules
{
	public CX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore" });
		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "SlateCore" });
	}
}

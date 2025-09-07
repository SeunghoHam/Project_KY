// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KY : ModuleRules
{
	public KY(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate","SlateCore",
			"Niagara","NiagaraAnimNotifies",
			/*
			"ProceduralMeshComponent","GeometryCore","GeometryFramework", // SkeletalMesh 자르기
			"NiagaraCore","RenderCore","RHI","DynamicMesh","GeometryAlgorithms","MeshConversion",
			"MeshDescription","MeshConversionEngineTypes","SkeletalMeshDescription",*/
			
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"KY",
			"KY/Variant_Horror",
			"KY/Variant_Horror/UI",
			"KY/Variant_Shooter",
			"KY/Variant_Shooter/AI",
			"KY/Variant_Shooter/UI",
			"KY/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

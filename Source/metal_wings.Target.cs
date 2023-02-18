// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class metal_wingsTarget : TargetRules
{
	public metal_wingsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("metal_wings");
	}
}
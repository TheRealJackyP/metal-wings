// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class metal_wingsEditorTarget : TargetRules
{
	public metal_wingsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("metal_wings");
	}
}
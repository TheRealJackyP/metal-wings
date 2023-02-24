using UnrealBuildTool;

public class MetalWingsCamera : ModuleRules
{
	public MetalWingsCamera(ReadOnlyTargetRules Target) : base(Target)
	{
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] {"Core", "CoreUObject", "Engine", "GameplayAbilities", "GameplayAbilitiesEditor", "GameplayTasks", "GameplayTasksEditor", "GameplayTags", "EnhancedInput"});
	}
}
using UnrealBuildTool;

public class MetalWingsCore : ModuleRules
{
    public MetalWingsCore(ReadOnlyTargetRules Target) : base(Target)
    {
        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            });
    }
}
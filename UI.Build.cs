using System.IO;
using UnrealBuildTool;

public class UI : ModuleRules
{
    public UI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Source/Public"),
                Path.Combine(ModuleDirectory, "Source/Classes")
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Source/Private")
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "RenderCore",
                "RHI"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
            }
        );
    }
}

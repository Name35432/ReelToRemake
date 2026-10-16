// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ReelToRemake : ModuleRules
{
	public ReelToRemake(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "XmlParser", "Json", "Slate", "SlateCore", "UMG", "MediaAssets" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty", "RshwLibrary", "Win64");
        	string DllName = "UnrealRSHW.exe";
        	string DllPath = Path.Combine(ThirdPartyPath, DllName);

		string tpp = Path.Combine(ModuleDirectory, "..", "ThirdParty", "ffmpeg", "Win64");
        	string dn = "ffmpeg.exe";
        	string dp = Path.Combine(tpp, dn);

		RuntimeDependencies.Add("$(BinaryOutputDir)/" + DllName, DllPath);
		RuntimeDependencies.Add("$(BinaryOutputDir)/" + dn, dp);
		
		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("DesktopPlatform");
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

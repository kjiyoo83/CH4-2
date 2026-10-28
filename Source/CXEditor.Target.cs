// 저작권은 Epic Games, Inc.에 있으며 모든 권리를 보유함

using UnrealBuildTool;
using System.Collections.Generic;

public class CXEditorTarget : TargetRules
{
	public CXEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("CX");
	}
}

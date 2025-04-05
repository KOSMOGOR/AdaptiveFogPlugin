#include "AdaptiveFogModule.h"
#include "AdaptiveFogLog.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "SceneViewExtension.h"


#define LOCTEXT_NAMESPACE "FAdaptiveFogModule"

void FAdaptiveFogModule::StartupModule()
{
	UE_LOG(AdaptiveFog, Log, TEXT("FAdaptiveFogModule startup"));

	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("AdaptiveFog"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/AdaptiveFog"), PluginShaderDir);
	
	FCoreDelegates::OnPostEngineInit.AddLambda([this]() {
		ViewExtension = FSceneViewExtensions::NewExtension<FAdaptiveFogSceneViewExtension>();
	});

}

void FAdaptiveFogModule::ShutdownModule()
{
	UE_LOG(AdaptiveFog, Log, TEXT("FAdaptiveFogModule shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAdaptiveFogModule, AdaptiveFog);

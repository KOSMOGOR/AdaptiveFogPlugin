#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "AdaptiveFogSceneViewExtension.h"

class FAdaptiveFogModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	TSharedPtr< class FAdaptiveFogSceneViewExtension, ESPMode::ThreadSafe > ViewExtension;
};

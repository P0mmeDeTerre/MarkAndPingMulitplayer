// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMarkAndPingMultiplayerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

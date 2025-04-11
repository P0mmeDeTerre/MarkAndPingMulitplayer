// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "MarkAndPingMultiplayerSettings.generated.h"


class UMarkUserWidget;

UCLASS(Config = MarkAndPingMultiplayer, DefaultConfig, meta=(DisplayName="Mark and Ping Multiplayer"))
class UMarkAndPingMultiplayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, config, Category = "Mark and Ping Settings")
	TSubclassOf<UMarkUserWidget> CustomWidgetClass;

	UMarkAndPingMultiplayerSettings();
};

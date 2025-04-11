// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "MarkAndPingMultiplayerSettings.h"
#include "MarkUserWidget.h"

UMarkAndPingMultiplayerSettings::UMarkAndPingMultiplayerSettings()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("WidgetBlueprint'/MarkAndPingMultiplayer/UI/MarkUI.MarkUI_C'"));

	if(WidgetClassFinder.Succeeded())
	{
		CustomWidgetClass = WidgetClassFinder.Class;
	}
	else
	{
		CustomWidgetClass = UMarkUserWidget::StaticClass();
	}
	
	
}

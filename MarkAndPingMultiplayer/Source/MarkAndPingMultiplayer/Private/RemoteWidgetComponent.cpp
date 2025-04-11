// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "RemoteWidgetComponent.h"

void URemoteWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if(auto* World = GetWorld())
	{
		OwnerPlayer = World->GetFirstLocalPlayerFromController();
	}
}


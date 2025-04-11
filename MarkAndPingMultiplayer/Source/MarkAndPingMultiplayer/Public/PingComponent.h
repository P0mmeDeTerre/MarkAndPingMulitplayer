// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "MarkUserWidget.h"
#include "MarkAndPingMultiplayerSettings.h"
#include "RemoteWidgetComponent.h"
#include "PingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegatePingComponentOnTeamUpdate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARKANDPINGMULTIPLAYER_API UPingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPingComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ping")
	URemoteWidgetComponent* GetPingWidgetComponent() const { return PingWidgetComponent; }

	UFUNCTION(BlueprintCallable, Category = "Ping")
	UMarkUserWidget* GetPingWidget() const { return PingWidget; }
	
	void UpdateMyId(int32 Id);
	int32 GetMyId() const { return MyId; }
	void CreatePingWidget();
	void UpdateIcon(UTexture2D* Icon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Team Update"))
	FDelegatePingComponentOnTeamUpdate OnTeamUpdateEvent;

	UPROPERTY(EditAnywhere, Category = "Ping")
	UMaterialInstance* IconMaterial;
	
private:
	UPROPERTY()
	TSubclassOf<UMarkUserWidget> WidgetClass;

	UPROPERTY()
	URemoteWidgetComponent* PingWidgetComponent = nullptr;

	UPROPERTY()
	UMarkUserWidget* PingWidget;

	UPROPERTY()
	int32 MyId = INDEX_NONE;
		
};

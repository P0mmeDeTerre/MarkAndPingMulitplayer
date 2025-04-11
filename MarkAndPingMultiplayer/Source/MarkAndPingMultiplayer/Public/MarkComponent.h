// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarkMisc.h"
#include "MarkedComponent.h"
#include "PingComponent.h"
#include "MarkUserWidget.h"
#include "DrawDebugHelpers.h"
#include "MarkAndPingMultiplayerSettings.h"
#include "RemoteWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "kismet/GameplayStatics.h"
#include "MarkAndPingMultiplayerBPLibrary.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "MarkComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMarkComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateMarkComponentOnTeamUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateMarkComponentOnPing);

USTRUCT(BlueprintType)
struct FMarkPlayerWidget
{
	GENERATED_BODY();

	UPROPERTY()
	int32 PlayerId;
	
	UPROPERTY()
	APawn* Pawn;

	UPROPERTY()
	UMarkUserWidget* Widget;

	UPROPERTY()
	EMarkType Type;

	FMarkPlayerWidget()
	{
		PlayerId = -1;
		Pawn = nullptr;
		Widget = nullptr;
		Type = EMarkType::Ping;
	}

	FMarkPlayerWidget(int32 InPlayerId, APawn* InPawn, UMarkUserWidget* InWidget, EMarkType InType)
	{
		PlayerId = InPlayerId;
		Pawn = InPawn;
		Widget = InWidget;
		Type = InType;
	}
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARKANDPINGMULTIPLAYER_API UMarkComponent : public UActorComponent
{
	GENERATED_BODY()

	UMarkComponent();
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Client, Reliable)
	void UpdateTeam(const TArray<FMarkTeamInfo>& NewTeams, int32 NewTeamId, int32 NewMyId);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMark(bool bFollowActor); 

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "General")
	void ClearWidgets(EMarkType Type);

	UFUNCTION(Client, Reliable)
	void PlayerControllerShowPing(int32 Id, FVector WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime);

	UFUNCTION(Client, Reliable)
	void PlayerControllerRemovePing();

	UFUNCTION(Client, Reliable)
	void ClientShowPing(int32 PlayerId, int32 Id, AActor* From, UMarkComponent* Component, FVector WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime);

	UFUNCTION(Client, Reliable)
	void ClientRemovePing(int32 PlayerId, int32 Id, AActor* From, UMarkComponent* Component);
	
	UFUNCTION(Client, Reliable)
	void ClientShowMark(int32 PlayerId, AActor* From, FVector WorldLocation, AActor* HitActor, UMarkedComponent* Component, int64 StartedTime, FLinearColor Color, bool bFollowActor, bool bForceColor = false);

	UFUNCTION(Client, Reliable)
	void ClientRemoveMark(UMarkedComponent* Component);
	
	UFUNCTION(BlueprintCallable, Category = "Mark")
	UMarkUserWidget* GetLocalPingWidget() const { return PingLocalWidget; }

	UFUNCTION(BlueprintCallable, Category = "Mark")
	UMaterialInstance* GetPingMaterialInstance() const { return PingMaterialInstance; }
	
	UFUNCTION(BlueprintCallable, Category = "Mark")
	FLinearColor GetPingColor() const { return PingColor; }

	UFUNCTION(BlueprintCallable, Category = "Mark")
	bool IsTeamPingColorEnabled() const { return bUseTeamPingColor; }

	UFUNCTION(BlueprintCallable, Category = "Mark")
	bool IsTeamMarkColorEnabled() const { return bUseTeamMarkColor; }

	UFUNCTION(BlueprintCallable, Category = "Mark")
	URemoteWidgetComponent* GetWidgetComponent() const { return PingWidgetComponent; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Update settings on player controller's widget and all other ping widgets"), Category = "Mark")
	void UpdateSettings(const FMarkWidgetSettings& Settings);
	
protected:
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Mark")
	void UpdatePingWidgetTeamColor(bool bUseTeamColor);

	UFUNCTION(BlueprintCallable, Category = "Mark")
	void UpdateMarkWidgetTeamColor(bool bUseTeamColor);

	UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Team Update"))
	FDelegateMarkComponentOnTeamUpdate OnTeamUpdateEvent;

	UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Ping"))
	FDelegateMarkComponentOnPing OnPingEvent;
	
	UPROPERTY(EditAnywhere, Category = "Mark")
	UMaterialInstance* PingMaterialInstance;
	
	UPROPERTY(EditAnywhere, Category = "Mark")
	FLinearColor PingColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, Category = "Mark")
	double Cooldown = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Mark")
	float Length = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Mark")
	bool bDebugDrawLine = false;

	UPROPERTY(EditAnywhere, Category = "Mark")
	bool bUseTeamPingColor = true;

	UPROPERTY(EditAnywhere, Category = "Mark")
	bool bUseTeamMarkColor = true;

	UPROPERTY(EditAnywhere, Category = "Mark")
	USoundBase* PingSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mark")
	FMarkWidgetSettings PingWidgetSettings;

private:

	UFUNCTION(BlueprintCallable, Category = "General")
	void PerformTrace(AActor* FromActor, bool bFollowActor);
	
	UFUNCTION(BlueprintCallable, Category = "General")
	void Ping(bool bFollowActor);
	
	UFUNCTION(Server, Reliable)
	void ServerShowPing(int32 PlayerId, AActor* From, URemoteWidgetComponent* Widget, FVector WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleMark(FVector WorldLocation, AActor* HitActor, AActor* From,UMarkedComponent* Component, int64 StartedTime, bool bFollowActor);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandlePing(FVector WorldLocation, AActor* From, UMarkComponent* Component, int64 StartedTime);

	bool IsLocallyControlled() const;
	bool UpdatePingLocalWidget();
	void ForceUpdatePingWidgetColor(FLinearColor Color);
	int DoesWidgetAlreadyExist(int32 PlayerId, EMarkType Type);
	int DoesMarkWidgetAlreadyExist(UMarkUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "General")
	void OnPing(int32 Id, UMarkUserWidget* PingWidget, FVector WorldLocation,FLinearColor Color, float Lifetime, int64 StartedTime);
	
	
	UPROPERTY()
	URemoteWidgetComponent* PingWidgetComponent = nullptr;

	UPROPERTY()
	UPingComponent* PingComponent = nullptr;

	UPROPERTY()
	TArray<FMarkPlayerWidget> PingWidgets; 

	UPROPERTY()
	UMarkUserWidget* PingLocalWidget;

	UPROPERTY()
	APlayerController* PC;
	
	UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;
	
	FTimerHandle InitTimerHandle;
	double LastPingTime = 0.f; 

};

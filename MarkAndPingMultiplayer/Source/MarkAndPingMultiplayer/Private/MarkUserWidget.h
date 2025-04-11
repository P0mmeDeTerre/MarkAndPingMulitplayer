// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarkedComponent.h"
#include "MarkMisc.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/Spacer.h"
#include "MarkUserWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMarkUserWidget, Log, All);

class UMarkedComponent;

UCLASS()
class UMarkUserWidget : public UUserWidget
{
	GENERATED_BODY()

	UMarkUserWidget(const FObjectInitializer& ObjectInitializer);

public:
	
	void OnMark(const FMarkData& MarkData);
	void OnPing(APawn* Pawn, APlayerController* PC, const FVector& WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime, bool bDestroyAfterLifetime = true);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetTextureSize(FVector2D Size) const;
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetIconTexture(UTexture2D* Texture) const;

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetIconColor(FLinearColor Color) const;
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetIconBorderColor(FLinearColor Color) const;

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetIconBackgroundColor(FLinearColor Color) const;
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetMaterialInstance(UMaterialInstance* MaterialInstance);
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void Hide();
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetOffset(FVector Offset);
	
	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetMaterialInstanceDynamic(UMaterialInstanceDynamic* MaterialInstance);
	
	FMarkWidgetSettings GetSettings()const { return Settings; }
	void SetSettings(const FMarkWidgetSettings& NewSettings) { Settings = NewSettings; ApplySettings(); }

	void ApplySettings() const;

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	UTexture2D* GetIconTexture() const;

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	UImage* GetMarkIcon() const { return MarkIcon; }

	UFUNCTION(BlueprintCallable, Category = "Mark Widget")
	void SetMarkIcon(UImage* Icon) { MarkIcon = Icon; }

	bool IsIconSet() const;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;


	// Blueprint event that can be called in this class
	UFUNCTION(BlueprintImplementableEvent, Category = "Mark Widget")
	void OnPingEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mark Widget")
	void OnMarkEvent();
	
private:

	UPROPERTY()
	FMarkData Mark;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MarkVerticalBox;

	UPROPERTY(meta = (BindWidget))
	UImage* MarkIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MarkText;

	UPROPERTY(meta = (BindWidget))
	USpacer* MarkSpacer;

	UPROPERTY(EditAnywhere, Category = "General")
	UMaterialInstance* IconMaterial; 

	UPROPERTY(EditAnywhere, Category = "General")
	UMaterialInstanceDynamic* IconMaterialInstance;
	
	UPROPERTY()
	URemoteWidgetComponent* WidgetComponent = nullptr;

	UPROPERTY()
	FTimerHandle TextUpdateTimerHandle;

	UPROPERTY()
	FTimerHandle LifetimeTimerHandle;

	UPROPERTY()
	FMarkWidgetSettings Settings;
	
	void Update();
	void StopTimers();

	friend void UMarkedComponent::SetupWidget() const;
};

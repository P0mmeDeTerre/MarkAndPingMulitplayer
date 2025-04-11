// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "MarkAndPingMultiplayerSettings.h"
#include "RemoteWidgetComponent.h"
#include "MarkMisc.h"
#include "MarkedComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMarkedComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateMarkedComponentOnMark);

class UMarkUserWidget;
struct FMarkData;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARKANDPINGMULTIPLAYER_API UMarkedComponent : public UActorComponent
{
	GENERATED_BODY()

	UMarkedComponent();
public:	
	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	UTexture2D* GetIcon() const { return Icon; }

	UFUNCTION(BlueprintCallable,  meta = (Tooltip = "Set Icon. This function update immediatly the icon."), Category = "Marked Component")
	void SetIcon(UTexture2D* NewIcon);
	
	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	float GetLifetime() const { return Lifetime; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Set Lifetime. (does not affect the current Mark)"), Category = "Marked Component")
	void SetLifetime(float NewLifetime) { Lifetime = NewLifetime; }

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	FLinearColor GetMarkColor() const { return MarkColor; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Set Color. (does not affect the current Mark)"), Category = "Marked Component")
	void SetMarkColor(FLinearColor NewMarkColor) { MarkColor = NewMarkColor; }
	
	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	bool GetIgnoreItself() const { return bIgnoreItself; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Set Ignore itself. (does not affect the current Mark)"), Category = "Marked Component")
	void SetIgnoreItself(bool bNewIgnoreItself) { bIgnoreItself = bNewIgnoreItself; }

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	FVector GetOffset() const { return Offset; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Set Offset. This function update immediatly the offset"), Category = "Marked Component")
	void SetOffset(FVector NewOffset);

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	FVector2D GetTextureSize() const { return TextureSize; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Set Texture Size. This function update immediatly the texture size"), Category = "Marked Component")
	void SetTextureSize(FVector2D NewTextureSize);

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	USoundBase* GetMarkSound() const { return MarkSound; }

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	void SetMarkSound(USoundBase* NewMarkSound) { MarkSound = NewMarkSound; }

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Update settings on the mark widget. All settings execept UpdateTime will immediatly be updated"), Category = "Marked Component")
	void UpdateSettings(const FMarkWidgetSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	UMarkUserWidget* GetMarkWidget() const { return MarkWidget; }

	UFUNCTION(BlueprintCallable, Category = "Marked Component")
	FMarkWidgetSettings GetMarkWidgetSettings() const { return MarkWidgetSettings; }
	
	void OnMark(const FMarkData& MarkData);
	void ClientRemoveMark();
	void SetupWidget() const;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Mark"))
    FDelegateMarkedComponentOnMark OnMarkEvent;

	UPROPERTY(EditAnywhere, Category = "Marked")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "Marked")
	float Lifetime;

	UPROPERTY(EditAnywhere, Category = "Marked")
	FLinearColor MarkColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "Marked")
	FVector Offset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Marked")
	FVector2D TextureSize = FVector2D(1, 1);

	UPROPERTY(EditAnywhere, Category = "Marked")
	USoundBase* MarkSound;

	UPROPERTY(EditAnywhere, Category = "Marked")
	bool bIgnoreItself = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marked")
	FMarkWidgetSettings MarkWidgetSettings;

private:

	void SetupWidgetComponent();
	
	UPROPERTY()
	URemoteWidgetComponent* WidgetComponent = nullptr;

	UPROPERTY()
	UMarkUserWidget* MarkWidget = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;
};

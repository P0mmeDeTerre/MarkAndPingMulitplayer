// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "MarkMisc.generated.h"

class UMarkedComponent;

USTRUCT(BlueprintType)
struct FMarkData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	APawn* Pawn = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	APlayerController* PC = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	UMarkedComponent* Component = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	AActor* HitActor = nullptr; 

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	FVector WorldLocation = FVector(); // if the ping is not following an actor, this is the location of the ping

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	FVector Offset = FVector(); // offset from the actor location

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	bool bFollowActor = false; // If true, the ping will follow the actor around ( ? need to get a reference to the actor )

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, Category = "Mark Data")
	int64 StartedTime = 0.0; // time when the mark started on the server. this is to sync the mark time with the client

	bool bDestroyAfterLifetime = true;
	
	FMarkData() = default;
	FMarkData(APawn* InPawn, APlayerController* InPC, UMarkedComponent* InComponent, AActor* InHitActor, FVector InWorldLocation, bool InbFollowActor, int64 InStartedTime, FLinearColor InColor)
	: Pawn(InPawn), PC(InPC), Component(InComponent), HitActor(InHitActor), WorldLocation(InWorldLocation), bFollowActor(InbFollowActor), Color(InColor), StartedTime(InStartedTime) {}
};

USTRUCT(BlueprintType)
struct  FMarkTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Team Info")
	FString Name = "Team";

	UPROPERTY(BlueprintReadOnly, Category = "Team Info")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category = "Team Info")
	float Lifetime = 5.0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Team Info")
	TArray<UTexture2D*> Icons = TArray<UTexture2D*>();
};

USTRUCT(BlueprintType)
struct FMarkWidgetSettings
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Offset of the widget"), Category = "Mark Widget Settings")
	FVector2D Offset = FVector2d(0.5, 0.5);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Size of the icon"), Category = "Mark Widget Settings")
	FVector2D IconSize = FVector2d(64, 64);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Space between the icon and the text"), Category = "Mark Widget Settings")
	float Space = 8.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Visibility of the text under the icon"), Category = "Mark Widget Settings")
	bool bIsTextVisible = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Number of decimal places to display (e.g. 2 will display '3.87 M')", ClampMin = "0", ClampMax = "5"), Category = "Mark Widget Settings")
	int32 DecimalPlaces = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Time interval for updating the widget" ), Category = "Mark Widget Settings")
	float UpdateTime = 0.1f;
	
};

UENUM()
enum EMarkUiTextType
{
	MUTT_INT UMETA(DisplayName = "Integer"),
	MUTT_FLOAT UMETA(DisplayName = "Float"),
};


UENUM(BlueprintType)
enum class EMarkType : uint8
{
	// Usable in blueprint
	Both UMETA(DisplayName = "Both"),
	Mark UMETA(DisplayName = "Mark"),
	Ping UMETA(DisplayName = "Ping")
};

UCLASS()
class MARKANDPINGMULTIPLAYER_API UMarkMisc : public UObject
{

	GENERATED_BODY()
	
	UMarkMisc();
	~UMarkMisc();
};

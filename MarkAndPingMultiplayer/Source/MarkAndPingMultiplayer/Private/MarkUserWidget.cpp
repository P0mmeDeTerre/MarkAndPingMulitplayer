// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "MarkUserWidget.h"
#include "MarkedComponent.h"
#include "Blueprint/WidgetTree.h"

DEFINE_LOG_CATEGORY(LogMarkUserWidget);

UMarkUserWidget::UMarkUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Canvas = nullptr;
	MarkVerticalBox = nullptr;
	MarkIcon = nullptr;
	MarkText = nullptr;
	MarkSpacer = nullptr;
	IconMaterialInstance = nullptr;
	
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconMaterialAsset(TEXT("MaterialInstanceConstant'/MarkAndPingMultiplayer/Materials/IconMat_Inst.IconMat_Inst'"));
	if (IconMaterialAsset.Succeeded())
	{
		IconMaterial = IconMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material not found. The mark wont be updated with the right icon and colors"));
	}

	
}

void UMarkUserWidget::OnMark(const FMarkData& MarkData)
{
	if(GetWorld())
	{
		SetVisibility(ESlateVisibility::Visible);

		if(WidgetComponent && MarkData.bFollowActor && MarkData.HitActor)
		{
			WidgetComponent->AttachToComponent(MarkData.HitActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}

		Mark = MarkData;
		
		SetOffset(MarkData.Offset);
		SetIconBorderColor(MarkData.Color);

		if(Mark.PC && Mark.PC->IsHidden())
		{
			Mark.PC->SetActorHiddenInGame(false);
		}
		
		GetWorld()->GetTimerManager().SetTimer(TextUpdateTimerHandle, [this]()
		{
			this->Update();
		}, Settings.UpdateTime, true, 0.0f);
		
		int64 ElapseTime = FDateTime::UtcNow().ToUnixTimestamp() - MarkData.StartedTime;
		float RealLifetime = MarkData.Component->GetLifetime() - ElapseTime;
		RealLifetime = FMath::Clamp(RealLifetime, 0.0f, MarkData.Component->GetLifetime());

		OnMarkEvent();
	
		GetWorld()->GetTimerManager().SetTimer(LifetimeTimerHandle, [this]()
		{
			Hide();
		}, RealLifetime, false);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("World is null. Cannot update the mark widget"));
	}
	
	
}

void UMarkUserWidget::OnPing(APawn* Pawn, APlayerController* PC,const FVector& WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime, bool bDestroyAfterLifetime)
{
	FMarkData MarkData;
	MarkData.Pawn = Pawn;
	MarkData.PC = PC;
	MarkData.WorldLocation = WorldLocation;
	MarkData.Color = Color;
	MarkData.StartedTime = StartedTime;
	MarkData.bFollowActor = false;
	MarkData.HitActor = nullptr;
	MarkData.Component = nullptr;
	MarkData.bDestroyAfterLifetime = bDestroyAfterLifetime;

	Mark = MarkData;

	SetIconColor(MarkData.Color);
	SetVisibility(ESlateVisibility::Visible);

	if(Mark.PC && Mark.PC->IsHidden())
	{
		Mark.PC->SetActorHiddenInGame(false);
	}
	
	GetWorld()->GetTimerManager().SetTimer(TextUpdateTimerHandle, [this]()
		{
			this->Update();
		}, Settings.UpdateTime, true, 0.0f);
	
	const int64 ElapseTime = FDateTime::UtcNow().ToUnixTimestamp() - MarkData.StartedTime;
	float RealLifetime = Lifetime - ElapseTime;
	RealLifetime = FMath::Clamp(RealLifetime, 0.0f, Lifetime);

	OnPingEvent();
	
	GetWorld()->GetTimerManager().SetTimer(LifetimeTimerHandle, [this]()
	{
		Hide();
	}, RealLifetime, false);
}

void UMarkUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMarkUserWidget::SetTextureSize(FVector2D Size) const
{
	if(IconMaterialInstance)
	{
		IconMaterialInstance->SetScalarParameterValue("TextureSizeX", Size.X);
		IconMaterialInstance->SetScalarParameterValue("TextureSizeY", Size.Y);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. TextureSize will not be set"));
	}
}

void UMarkUserWidget::SetIconTexture(UTexture2D* Texture) const
{
	if(IconMaterialInstance)
	{
		IconMaterialInstance->SetTextureParameterValue("Texture", Texture);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. Icon will not be set"));
	}
}

void UMarkUserWidget::SetIconColor(FLinearColor Color) const
{
	if(IconMaterialInstance)
	{
		IconMaterialInstance->SetVectorParameterValue("Color", Color);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. Icon will not be set"));
	}
}

void UMarkUserWidget::SetIconBorderColor(FLinearColor Color) const
{
	if(IconMaterialInstance)
	{
		IconMaterialInstance->SetVectorParameterValue("BorderColor", Color);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. BorderColor will not be set"));
	}
}

void UMarkUserWidget::SetIconBackgroundColor(FLinearColor Color) const
{
	if(IconMaterialInstance)
	{
		IconMaterialInstance->SetVectorParameterValue("BackgroundColor", Color);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. BackgroundColor will not be set"));
	}
}

void UMarkUserWidget::SetMaterialInstance(UMaterialInstance* MaterialInstance)
{
	IconMaterial = MaterialInstance;
	IconMaterialInstance = UMaterialInstanceDynamic::Create(IconMaterial, this);
	if(IconMaterialInstance && MarkIcon)
	{
		MarkIcon->SetBrushFromMaterial(IconMaterialInstance);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set. Icon, BorderColor and BackgroundColor will not be set"));
	}
}

void UMarkUserWidget::Hide()
{
	StopTimers();
	SetVisibility(ESlateVisibility::Hidden);
	
	if(WidgetComponent && Mark.bFollowActor)
	{
		WidgetComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	if(Mark.bDestroyAfterLifetime)
	{
		NativeDestruct();
	}
}

void UMarkUserWidget::SetOffset(FVector Offset)
{
	Mark.Offset = Offset;
	WidgetComponent->SetWorldLocation(Mark.WorldLocation +  Mark.Offset);
}

void UMarkUserWidget::SetMaterialInstanceDynamic(UMaterialInstanceDynamic* MaterialInstance)
{
	IconMaterialInstance = MaterialInstance;

	if(IconMaterialInstance && MarkIcon)
	{
		MarkIcon->SetBrushFromMaterial(IconMaterialInstance);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material Instance not set."));
	}
}

void UMarkUserWidget::ApplySettings() const
{
	if(MarkSpacer)
	{
		MarkSpacer->SetSize(FVector2D(1.0f, Settings.Space));
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("MarkSpacer is null. Cannot apply the settings"));
	}
	
	if(MarkIcon)
	{
		MarkIcon->SetDesiredSizeOverride(Settings.IconSize);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("MarkSpacer is null. Cannot apply the settings"));
	}
	
	if(MarkText)
	{
		MarkText->SetVisibility(Settings.bIsTextVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("MarkText is null. Cannot apply the settings"));
	}
}

UTexture2D* UMarkUserWidget::GetIconTexture() const
{
	//IconMaterialInstance->SetTextureParameterValue("Texture", Texture);
	UTexture* IconTexture = nullptr;
	IconMaterialInstance->GetTextureParameterValue( FHashedMaterialParameterInfo("Texture"), IconTexture);
	return Cast<UTexture2D>(IconTexture);
}

bool UMarkUserWidget::IsIconSet() const
{
	// CHECK ON THE ICON MATERIAL INSTANCE IF THE TEXTURE IS SET
	UTexture* IconTexture = nullptr;

	IconMaterialInstance->GetTextureParameterValue( FHashedMaterialParameterInfo("Texture"), IconTexture);

	UE_LOG(LogMarkUserWidget, Warning, TEXT("IconTexture: %s"), *IconTexture->GetName());

	return IconTexture != nullptr;
}

void UMarkUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SetVisibility(ESlateVisibility::Hidden);
	
	Canvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas")));
	MarkVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("MarkVerticalBox")));
	MarkIcon = Cast<UImage>(GetWidgetFromName(TEXT("MarkIcon")));
	MarkText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MarkText")));
	MarkSpacer = Cast<USpacer>(GetWidgetFromName(TEXT("MarkSpacer")));
	
	SetMaterialInstance(IconMaterial);
	
	if(MarkIcon)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MarkIcon->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		}
	}
}

void UMarkUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	StopTimers();
}

void UMarkUserWidget::Update()
{
	if(Mark.PC)
	{
		APawn* Pawn = Mark.PC->GetPawn();
		
		if(!Mark.Pawn || Mark.Pawn != Pawn)
		{
			if(!Pawn)
			{
				UE_LOG(LogMarkUserWidget, Warning, TEXT("Pawn is null after trying to update the reference. Mark widget will be hidden"));
				Hide();
				return;
			}
			
			// if pawn changed successfully, check if the new pawn is the same as the marked actor
			if(Mark.HitActor && Mark.HitActor == Cast<AActor>(Pawn))
			{
				UE_LOG(LogMarkUserWidget, Display, TEXT("Pawn is the same as the marked actor. Hiding the mark widget"));
				Hide();
				return;
			}

			UE_LOG(LogMarkUserWidget, Display, TEXT("Pawn is null or changed. Updating the pawn reference"));
			Mark.Pawn = Pawn;
		}
	
		if(Settings.bIsTextVisible)
		{
			FVector Location;

			if(Mark.bFollowActor)
			{
				// Re attach the widget if needed
				if(Mark.HitActor && WidgetComponent->GetAttachParent() != Mark.HitActor->GetRootComponent())
				{
					WidgetComponent->AttachToComponent(Mark.HitActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				}
				
				if(Mark.HitActor)
				{
					Location = Mark.HitActor->GetActorLocation();
				}
				else
				{
					UE_LOG(LogMarkUserWidget, Warning, TEXT("FollowActor is true but Marked actor is null. Using WorldLocation instead of the actor location and setting FollowActor to false"));
					Mark.bFollowActor = false;
					Location = Mark.WorldLocation;
				}
			}
			else
			{
				Location = Mark.WorldLocation;
			}

			if(Mark.Pawn)
			{
				if(MarkText)
				{
					const float Distance = FVector::Dist(Mark.Pawn->GetActorLocation(), Location) / 100.0f;
				
					MarkText->SetText(FText::FromString(FString::Printf(TEXT("%.*f M"), Settings.DecimalPlaces, Distance)));
				}
				else
				{
					UE_LOG(LogMarkUserWidget, Warning, TEXT("MarkText is null. Cannot update the distance text"));
				}
			}
			else
			{
				UE_LOG(LogMarkUserWidget, Warning, TEXT("Pawn is null. Cannot calculate distance to the marked actor"));
			}
		}
	}
}

void UMarkUserWidget::StopTimers()
{
	GetWorld()->GetTimerManager().ClearTimer(TextUpdateTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LifetimeTimerHandle);
}

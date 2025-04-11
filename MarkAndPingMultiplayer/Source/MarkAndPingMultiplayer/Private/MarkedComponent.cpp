// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "MarkedComponent.h"
#include "MarkUserWidget.h"

DEFINE_LOG_CATEGORY(LogMarkedComponent);

UMarkedComponent::UMarkedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetupWidgetComponent();
	SetIsReplicatedByDefault(true);
}

void UMarkedComponent::SetIcon(UTexture2D* NewIcon)
{
	Icon = NewIcon;
	SetupWidget();
}

void UMarkedComponent::BeginPlay()
{
	Super::BeginPlay();

	const UMarkAndPingMultiplayerSettings* Settings = GetDefault<UMarkAndPingMultiplayerSettings>();

	if(Settings)
	{
		WidgetClass = Settings->CustomWidgetClass;
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Settings not found"));
	}
}

void UMarkedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMarkedComponent::SetOffset(FVector NewOffset)
{
	Offset = NewOffset;
	MarkWidget->SetOffset(Offset);
}

void UMarkedComponent::SetTextureSize(FVector2D NewTextureSize)
{
	TextureSize = NewTextureSize;
	SetupWidget();
}

void UMarkedComponent::UpdateSettings(const FMarkWidgetSettings& Settings)
{
	MarkWidgetSettings = Settings;
	if(WidgetComponent)
	{
		if(MarkWidget)
		{
			MarkWidget->SetSettings(Settings);
		}
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Widget Component is null. Mark Widget will not be displayed"));
	}
}

void UMarkedComponent::ClientRemoveMark()
{
	if(MarkWidget)
	{
		MarkWidget->Hide();
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Widget not found. Mark Widget will not be displayed"));
	}
}

void UMarkedComponent::OnMark(const FMarkData& MarkData)
{
	if (WidgetComponent)
	{
		FMarkData Data = MarkData;
		Data.Offset = Offset;
		
		if(!WidgetComponent->GetWidget())
		{
			MarkWidget = CreateWidget<UMarkUserWidget>(GetWorld(), WidgetClass);
            WidgetComponent->SetWidget(MarkWidget);
			SetupWidget();
			MarkWidget->SetSettings(MarkWidgetSettings);
		}

		if(MarkWidget)
		{
			MarkWidget->OnMark(Data);
			UGameplayStatics::PlaySound2D(GetWorld(), MarkSound);
			OnMarkEvent.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Widget Component is null. Mark Widget will not be displayed"));
	}
}

void UMarkedComponent::SetupWidget() const
{
	if (MarkWidget)
	{
		MarkWidget->SetIconTexture(Icon);
		MarkWidget->SetTextureSize(TextureSize);
		MarkWidget->SetIsEnabled(true);
		
		MarkWidget->WidgetComponent = WidgetComponent;
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Widget not found. Mark Widget will not be displayed %s"), *WidgetComponent->GetOwner()->GetName());
	}
}

void UMarkedComponent::SetupWidgetComponent()
{
	WidgetComponent = CreateDefaultSubobject<URemoteWidgetComponent>(TEXT("WidgetComponent"));
	
	if(WidgetComponent)
	{
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetIsReplicated(true);
	}
	else
	{
		UE_LOG(LogMarkedComponent, Warning, TEXT("Widget Component not created. Mark Widget will not be displayed"));
	}
}


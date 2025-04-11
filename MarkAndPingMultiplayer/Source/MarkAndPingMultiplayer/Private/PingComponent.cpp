// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "PingComponent.h"
#include "MarkUserWidget.h"
#include "Components/WidgetComponent.h"
#include "MarkAndPingMultiplayerBPLibrary.h"

// Sets default values for this component's properties
UPingComponent::UPingComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> IconMaterialAsset(TEXT("MaterialInstanceConstant'/MarkAndPingMultiplayer/Materials/PingMat_Inst.PingMat_Inst'"));

	if (IconMaterialAsset.Succeeded())
	{
		IconMaterial = IconMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogMarkUserWidget, Warning, TEXT("Icon Material not found. The mark wont be updated with the right icon and colors"));
	}
	
	PingWidgetComponent = CreateDefaultSubobject<URemoteWidgetComponent>(TEXT("WidgetComponent"));

	if(PingWidgetComponent)
	{
		PingWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		PingWidgetComponent->SetIsReplicated(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget Component not created. Mark Widget will not be displayed"));
	}
}

void UPingComponent::UpdateMyId(int32 Id)
{
	MyId = Id;

	OnTeamUpdateEvent.Broadcast();
}

void UPingComponent::CreatePingWidget()
{
	PingWidget = CreateWidget<UMarkUserWidget>(GetWorld(), WidgetClass);
	if(PingWidget)
	{
		PingWidget->SetMaterialInstance(IconMaterial);
		PingWidgetComponent->SetWidget(PingWidget);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget Component not created. Mark Widget will not be displayed"));
	}
}

void UPingComponent::UpdateIcon(UTexture2D* Icon)
{
	if(PingWidget && Icon)
	{
		PingWidget->SetIconTexture(Icon);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget not found. Mark Widget will not be displayed"));
	}
	
}

void UPingComponent::BeginPlay()
{
	Super::BeginPlay();

	const UMarkAndPingMultiplayerSettings* Settings = GetDefault<UMarkAndPingMultiplayerSettings>();

	if(Settings)
	{
		WidgetClass = Settings->CustomWidgetClass;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Settings not found"));
	}
	
	if(PingWidgetComponent)
	{
		PingWidgetComponent->SetWidgetClass(WidgetClass);
	}

	CreatePingWidget();
}


// Called every frame
void UPingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


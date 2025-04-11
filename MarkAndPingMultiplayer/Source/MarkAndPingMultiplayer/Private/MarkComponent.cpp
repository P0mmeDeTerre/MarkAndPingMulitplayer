// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "MarkComponent.h"
#include "PingComponent.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY(LogMarkComponent);

UMarkComponent::UMarkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PingIconFinder(TEXT("MaterialInstanceConstant '/MarkAndPingMultiplayer/Materials/PingMat_Inst.PingMat_Inst'"));
	
	if(PingIconFinder.Succeeded())
	{
		PingMaterialInstance = PingIconFinder.Object;
	}
	
	PingWidgetComponent = CreateDefaultSubobject<URemoteWidgetComponent>(TEXT("WidgetComponent"));

	if(PingWidgetComponent)
	{
		PingWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		PingWidgetComponent->SetVisibility(true);
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Widget Component not created. Mark Widget will not be displayed"));
	}
}

void UMarkComponent::ServerMark_Implementation(bool bFollowActor)
{
	if (!GetOwner())
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("SERVER  : Mark failed: Owner is null"));
		return;
	}
	
	PerformTrace(GetOwner(), bFollowActor);
}

bool UMarkComponent::ServerMark_Validate(bool bFollowActor)
{
	return true;
}

void UMarkComponent::ClearWidgets_Implementation(EMarkType Type)
{
	for(FMarkPlayerWidget& PlayerWidget : PingWidgets)
	{
		if(PlayerWidget.Widget)
		{
			if(PlayerWidget.Type == Type || Type == EMarkType::Both)
			{
				PlayerWidget.Widget->Hide();
			}
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("Ping Widget not found"));
		}
	}
	
	if(!PC)
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Player Controller not found"));
		return;
	}
	
	if(PingComponent && PingComponent->GetPingWidget() && (Type == EMarkType::Both || Type == EMarkType::Ping))
	{
		PingComponent->GetPingWidget()->Hide();
	}

	PingWidgets.Empty();
}

void UMarkComponent::ClientRemoveMark_Implementation(UMarkedComponent* Component)
{
	if(Component)
	{
		Component->ClientRemoveMark();
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Mark Component not found"));
	}	
}

void UMarkComponent::UpdateSettings(const FMarkWidgetSettings& Settings)
{
	PingWidgetSettings = Settings;

	PingLocalWidget->SetSettings(Settings);

	for(const FMarkPlayerWidget& PlayerWidget : PingWidgets)
	{
		if(PlayerWidget.Widget && PlayerWidget.Type == EMarkType::Ping)
		{
			PlayerWidget.Widget->SetSettings(Settings);
		}
	}
}

// Called when the game starts
void UMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	const UMarkAndPingMultiplayerSettings* Settings = GetDefault<UMarkAndPingMultiplayerSettings>();

	if(Settings)
	{
		WidgetClass = Settings->CustomWidgetClass;
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Settings not found"));
	}

	if(PingWidgetComponent)
	{
		PingWidgetComponent->SetWidgetClass(WidgetClass);
		PingWidgetComponent->SetWidget(nullptr);
	}

	if(GetOwner() && GetWorld())
	{
		PC = GetOwner()->GetWorld()->GetFirstPlayerController();
	}
	
	if(PC)
	{
		PingComponent = PC->FindComponentByClass<UPingComponent>();
	}
	
	UpdatePingLocalWidget();
}

void UMarkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UMarkComponent::UpdatePingWidgetTeamColor(bool bUseTeamColor)
{
	bUseTeamPingColor = bUseTeamColor;

	ForceUpdatePingWidgetColor(bUseTeamPingColor ? UMarkAndPingMultiplayerBPLibrary::GetMyTeam().Color : PingColor);
}

void UMarkComponent::UpdateMarkWidgetTeamColor(bool bUseTeamColor)
{
	bUseTeamMarkColor = bUseTeamColor;

	for(FMarkPlayerWidget& PlayerWidget : PingWidgets)
	{
		if(PlayerWidget.Widget && PlayerWidget.Type == EMarkType::Mark)
		{
			PlayerWidget.Widget->SetIconColor(bUseTeamMarkColor ? UMarkAndPingMultiplayerBPLibrary::GetMyTeam().Color : PingColor);
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("Mark Widget not found"));
		}
	}
}

void UMarkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMarkComponent::PlayerControllerRemovePing_Implementation()
{
	if(PingLocalWidget)
	{
		PingLocalWidget->Hide();	
	}
}

void UMarkComponent::ClientRemovePing_Implementation(int32 PlayerId, int32 Id, AActor* From, UMarkComponent* Component)
{
	if(Component)
	{
		if(Component->GetWidgetComponent() && Component->GetWidgetComponent()->GetWidget())
		{
			UMarkUserWidget* Widget = Cast<UMarkUserWidget>(Component->GetWidgetComponent()->GetWidget());
			if(Widget)
			{
				Widget->Hide();
			}
		}
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Mark Component not found"));
	}
}

void UMarkComponent::UpdateTeam_Implementation(const TArray<FMarkTeamInfo>& NewTeams, int32 NewTeamId, int32 NewMyId)
{
	if(IsLocallyControlled())
	{
		UMarkAndPingMultiplayerBPLibrary::UpdateTeam(NewTeams, NewTeamId);
		
		if(PC)
		{
			if(PingComponent)
			{
				PingComponent->UpdateMyId(NewMyId);
				UTexture2D* Icon = const_cast<UTexture2D*>(UMarkAndPingMultiplayerBPLibrary::GetMemberIcon(NewMyId));
				PingComponent->UpdateIcon(Icon);
				OnTeamUpdateEvent.Broadcast();
			}
			else
			{
				UE_LOG(LogMarkComponent, Warning, TEXT("Ping Component not found"));
			}
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("Player Controller not found or Player State not valid"));
		}
	}
}

void UMarkComponent::Ping(bool bFollowActor)
{
	const double CurrentTime = FDateTime::Now().ToUnixTimestamp();

	if(CurrentTime >= LastPingTime + Cooldown)
	{
		LastPingTime = CurrentTime;
		
		if(GetOwner()->HasAuthority())
		{
			PerformTrace(GetOwner(), bFollowActor);
		}
		else
		{
			UE_LOG(LogMarkComponent, Display, TEXT("CLIENT : Sending Mark Request to Server"));
			ServerMark(bFollowActor);
		}

	}
}

bool UMarkComponent::ServerHandlePing_Validate(FVector WorldLocation, AActor* From, UMarkComponent* Component, int64 StartedTime)
{
	return true;
}

void UMarkComponent::ServerShowPing_Implementation(int32 PlayerId, AActor* From, URemoteWidgetComponent* Widget,
	FVector WorldLocation, FLinearColor Color, float Lifetime, int64 StartedTime)
{
	//@todo delete this
}

bool UMarkComponent::IsLocallyControlled() const
{
	if(PC)
	{
		return PC->IsLocalController();
	}
	
	return false;
}

bool UMarkComponent::UpdatePingLocalWidget()
{
	if(PingComponent)
	{
		PingLocalWidget = PingComponent->GetPingWidget();
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("Ping Component not found"));
	}
	
	return PingLocalWidget != nullptr;
}

void UMarkComponent::ForceUpdatePingWidgetColor(FLinearColor Color)
{
	for(FMarkPlayerWidget& PlayerWidget : PingWidgets)
	{
		if(PlayerWidget.Widget && PlayerWidget.Type == EMarkType::Ping)
		{
			PlayerWidget.Widget->SetIconColor(Color);
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("Ping Widget not found"));
		}
	}

	if(IsLocallyControlled())
	{
		if(PingLocalWidget)
		{
			PingLocalWidget->SetIconColor(Color);
		}
		else if(UpdatePingLocalWidget())
		{
			PingLocalWidget->SetIconColor(Color);
		}
	}
}

int UMarkComponent::DoesWidgetAlreadyExist(int32 PlayerId, EMarkType Type)
{
	int i = 0;
	while(i < PingWidgets.Num() && (PingWidgets[i].PlayerId != PlayerId || PingWidgets[i].Type != Type))
	{
		i++;
	}

	if(i < PingWidgets.Num())
	{
		return i;
	}
	
	return INDEX_NONE;
}

int UMarkComponent::DoesMarkWidgetAlreadyExist(UMarkUserWidget *Widget)
{
	int i = 0;
	while(i < PingWidgets.Num() && PingWidgets[i].Widget != Widget)
	{
		i++;
	}

	if(i < PingWidgets.Num())
	{
		return i;
	}
	
	return INDEX_NONE;
}

void UMarkComponent::OnPing(int32 Id, UMarkUserWidget* PingWidget, FVector WorldLocation, FLinearColor Color, float Lifetime,
                            int64 StartedTime)
{
	if(PingWidgetComponent)
	{
		PingWidgetComponent->SetWorldLocation(WorldLocation);
	}
	
	if(PingWidget && PC)
	{
		APawn* Pawn = PC->GetPawn();
		PingWidgetComponent->SetWidget(PingWidget);
		
		if(!bUseTeamPingColor)
		{
			Color = PingColor;
		}
		
		PingWidget->SetMaterialInstance(PingMaterialInstance);
		
		UTexture2D* Icon = const_cast<UTexture2D*>(UMarkAndPingMultiplayerBPLibrary::GetMemberIcon(Id));
		PingWidget->SetIconTexture(Icon);
		PingWidget->SetSettings(PingWidgetSettings);
		PingWidget->OnPing(Pawn, PC, WorldLocation, Color, Lifetime, StartedTime);
		UGameplayStatics::PlaySound2D(GetWorld(), PingSound);
		OnPingEvent.Broadcast();
	}
}

void UMarkComponent::PlayerControllerShowPing_Implementation(int32 Id, FVector WorldLocation, FLinearColor Color, float Lifetime,
                                                             int64 StartedTime)
{
	if(PC)
	{
		APawn* Pawn = PC->GetPawn();
		
		if(PingComponent)
		{
			PingComponent->GetPingWidgetComponent()->SetWorldLocation(WorldLocation);
			PingLocalWidget = PingComponent->GetPingWidget();
			
			if(PingLocalWidget)
			{
				if(!bUseTeamPingColor)
				{
					Color = PingColor;
				}
				
				PingLocalWidget->SetSettings(PingWidgetSettings);
				PingLocalWidget->OnPing(Pawn, PC, WorldLocation, Color, Lifetime, StartedTime, false);
				UGameplayStatics::PlaySound2D(GetWorld(), PingSound);
				OnPingEvent.Broadcast();
			}
			else
			{
				UE_LOG(LogMarkComponent, Warning, TEXT("CLIENT : Widget not found. Failed to show the ping"));
			}
		}
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("CLIENT : Player Controller not found. Failed to show the ping"));
	}
	
	
}

void UMarkComponent::ClientShowPing_Implementation(int32 PlayerId, int32 Id, AActor* From, UMarkComponent* Component, FVector WorldLocation, FLinearColor Color, float Lifetime,
                                                   int64 StartedTime)
{
	if(GetOwner() && From)
	{
		UMarkUserWidget* Widget;
		
		int WidgetIndex = DoesWidgetAlreadyExist(PlayerId, EMarkType::Ping);
		if(WidgetIndex != INDEX_NONE)
		{
			// If widget is valid but owner changed, recreate the widget
			if(PingWidgets[WidgetIndex].Widget && PingWidgets[WidgetIndex].Pawn != From)
			{
				PingWidgets[WidgetIndex].Widget->Hide();
				PingWidgets[WidgetIndex].Widget = nullptr;
				PingWidgets.RemoveAt(WidgetIndex);

				// Recreate the widget
				Widget = CreateWidget<UMarkUserWidget>(GetWorld(), PingWidgetComponent->GetWidgetClass());
				PingWidgets.Add(FMarkPlayerWidget(PlayerId, Cast<APawn>(From), Widget, EMarkType::Ping));
			}
			else
			{
				Widget = PingWidgets[WidgetIndex].Widget;
			}
		}
		else
		{
			Widget = CreateWidget<UMarkUserWidget>(GetWorld(), PingWidgetComponent->GetWidgetClass());
			PingWidgets.Add(FMarkPlayerWidget(PlayerId, Cast<APawn>(From), Widget, EMarkType::Ping));
		}
		
		if(Widget)
		{
			Widget->SetSettings(PingWidgetSettings);
			Component->OnPing(Id, Widget, WorldLocation, Color, Lifetime, StartedTime);
			UGameplayStatics::PlaySound2D(GetWorld(), PingSound);
			OnPingEvent.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("CLIENT : Owner is null"));
	}
}

void UMarkComponent::ClientShowMark_Implementation(int32 PlayerId, AActor* From, FVector WorldLocation, AActor* HitActor, UMarkedComponent* Component,
                                                   int64 StartedTime, FLinearColor Color, bool bFollowActor, bool bForceColor)
{
	if(Component)
	{
		if(!PC)
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("CLIENT : Player Controller not found"));
			return;
		}
		APawn* Pawn = PC->GetPawn();

		if(Component->GetIgnoreItself() && HitActor == Pawn)
		{
			UE_LOG(LogMarkComponent, Display, TEXT("CLIENT : Marked Actor is the same as the owner. Ignoring"));
			return;
		}

		if(!bUseTeamMarkColor && !bForceColor)
		{	
			Color = Component->GetMarkColor();
		}
		
		const FMarkData Data = FMarkData(Pawn, PC, Component, HitActor, HitActor->GetActorLocation(), bFollowActor, StartedTime, Color);
		Component->OnMark(Data);

		const int WidgetIndex = DoesMarkWidgetAlreadyExist(Cast<UMarkUserWidget>(Component->GetMarkWidget()));

		if(WidgetIndex == INDEX_NONE)
		{
			PingWidgets.Add(FMarkPlayerWidget(PlayerId, Cast<APawn>(From), Cast<UMarkUserWidget>(Component->GetMarkWidget()), EMarkType::Mark));
		}
	}
	else
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("CLIENT : Marked Component not found"));
	}
}

void UMarkComponent::ServerHandlePing_Implementation(FVector WorldLocation, AActor* From, UMarkComponent* Component, int64 StartedTime)
{
	UE_LOG(LogMarkComponent, Display, TEXT("SERVER : Handle Ping"));

	APawn* Pawn = Cast<APawn>(From);
	FMarkTeamInfo Team;
	int32 TeamId = INDEX_NONE;

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	
	if(Pawn)
	{
		if(PlayerController)
		{
			Team = UMarkAndPingMultiplayerBPLibrary::GetTeamMember(PlayerController->GetPlayerState<APlayerState>(), TeamId);
		}
	}

	if(TeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Player team not found"));
		return;
	}

	UMarkComponent* FromMarkComponent = Component;
	const TArray<TArray<APlayerState*>>& Members = UMarkAndPingMultiplayerBPLibrary::GetMembers();
	
	int32 FromPlayerId = PlayerController->GetPlayerState<APlayerState>()->GetPlayerId();
	TArray<TArray<APlayerState*>> PlayerId = UMarkAndPingMultiplayerBPLibrary::GetMembers();

	// Search for playerId
	int i = 0;
	int j = 0;

	// Find the index of the FromPlayerId in the members
	while(i < PlayerId.Num() && j < PlayerId[i].Num() && PlayerId[i][j]->GetPlayerId() != FromPlayerId)
	{
		j = 0;
		while(j < PlayerId[i].Num() && PlayerId[i][j]->GetPlayerId() != FromPlayerId)
		{
			j++;
		}

		if(j >= PlayerId[i].Num())
		{
			i++;
		}
	}
	
	for (const APlayerState* Member : Members[TeamId])
	{
		APawn* MemberActor = Member->GetPawn();
		
		if (MemberActor)
		{
			UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
			if (MarkComponent)
			{
				if(FromPlayerId == Member->GetPlayerId())
				{
					PlayerControllerShowPing(j, WorldLocation, Team.Color, Team.Lifetime, StartedTime);
				}
				else
				{
					MarkComponent->ClientShowPing(FromPlayerId, j, From, FromMarkComponent, WorldLocation, Team.Color, Team.Lifetime, StartedTime);
				}
			}
			else
			{
				UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Mark Component not found for %s"), *Member->GetPlayerName());
			}
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Member Actor not found"));
		}
	}
}

void UMarkComponent::ServerHandleMark_Implementation(FVector WorldLocation, AActor* HitActor, AActor* From, UMarkedComponent* Component, int64 StartedTime, bool bFollowActor)
{

	UE_LOG(LogMarkComponent, Display, TEXT("SERVER  : Handle mark"));
	
	const TArray<TArray<APlayerState*>>& Members = UMarkAndPingMultiplayerBPLibrary::GetMembers();
	
	int TeamIndex = 0;
	const APlayerController* PlayerController = Cast<APlayerController>(From->GetOwner());

	FMarkTeamInfo TeamInfo =  UMarkAndPingMultiplayerBPLibrary::GetTeamMember(PlayerController->GetPlayerState<APlayerState>(), TeamIndex);

	if(TeamIndex == Members.Num())
	{
		UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Player team not found"));
		return;
	}
	
	for (APlayerState* Member : Members[TeamIndex])
	{
		AActor* MemberActor = Member->GetPawn();
		if (MemberActor)
		{
			int32 FromPlayerId = PlayerController->GetPlayerState<APlayerState>()->GetPlayerId();
			UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
			if (MarkComponent)
			{
				MarkComponent->ClientShowMark(FromPlayerId, From, WorldLocation, HitActor, Component, StartedTime, TeamInfo.Color, bFollowActor);
			}
			else
			{
				UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Mark Component not found for %s"), *Member->GetPlayerName());
			}
		}
		else
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Member Actor not found"));
		}
	}
}

bool UMarkComponent::ServerHandleMark_Validate(FVector WorldLocation, AActor* HitActor, AActor* From, UMarkedComponent* Component, int64 StartedTime, bool bAll)
{
	return true;
}

void UMarkComponent::PerformTrace(AActor* FromActor, bool bFollowActor)
{

	double StartTime = FPlatformTime::Seconds();
	
	if(GetWorld() && GetOwner()->HasAuthority())
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(FromActor);
		
		UCameraComponent* CameraComponent = FromActor->FindComponentByClass<UCameraComponent>();

		if(!CameraComponent)
		{
			UE_LOG(LogMarkComponent, Warning, TEXT("SERVER : Camera Component not found"));
			return;
		}
		
		const FVector& Start = CameraComponent->GetComponentLocation();
		const FVector& End = CameraComponent->GetComponentRotation().Vector() * Length + Start;
		
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);
		
		if(WITH_EDITOR && bDebugDrawLine)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 1.0f);
		}
		
		
		if(HitResult.GetActor() == nullptr)
		{
			UE_LOG(LogMarkComponent, Display, TEXT("SERVER : Hit Result is null"));
			return;
		}
		
		UE_LOG(LogMarkComponent, Display, TEXT("SERVER : Performing Trace"));
		
		UMarkedComponent* MarkedComponent = HitResult.GetActor()->FindComponentByClass<UMarkedComponent>();

		const FVector Location = HitResult.Location;
		const int64 StartedTime = FDateTime::UtcNow().ToUnixTimestamp();
		
		if(MarkedComponent)
		{
			ServerHandleMark(Location, HitResult.GetActor(), FromActor, MarkedComponent, StartedTime, bFollowActor);
		}
		else
		{
			UMarkComponent* MarkComponent = FromActor->GetComponentByClass<UMarkComponent>();
			ServerHandlePing(Location,FromActor, MarkComponent, StartedTime);
		}

		double EndTime = FPlatformTime::Seconds();
		double ElapsedTime = (EndTime - StartTime) * 1000.0;

		UE_LOG(LogTemp, Log, TEXT("Execution time: %f ms"), ElapsedTime);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"

#include "MyGameModeBase.h"
#include "MyGameStateBase.h"
#include "PlayerController/MyPlayerController.h"
#include "Weapon/WeaponBase.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandMesh");
	
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandMesh");
	
	/*Body = CreateDefaultSubobject<USkeletalMeshComponent>("Body");
	Body->SetupAttachment(RootComponent);
	Body->SetRelativeLocation(FVector(0, 0, -90));
	Body->SetRelativeRotation(FRotator(0, -90, 0));*/
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false) return;
	
	if (AMyPlayerController* OwningController = GetController<AMyPlayerController>())
	{
		OwningController->AddPlayerToGameState();
	}
	
	/*GetWorld()->GetGameState<AMyGameStateBase>()->AddPlayer(this);

	// 타겟 설정(내가 아닌 world 내 Character)
	GetWorldTimerManager().SetTimer(FindTargetTimerHande, FTimerDelegate::CreateLambda([this]()
	{
		Target = GetWorld()->GetGameState<AMyGameStateBase>()->GetOtherCharacter(this);
		if (Target)
		{
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *Target->GetActorNameOrLabel());
			GetWorld()->GetTimerManager().ClearTimer(FindTargetTimerHande);
		}
	}), 1.0f, true);*/

	// GameMode 추가

	GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
}

bool ACharacterBase::CheckFall()
{
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start - GetActorUpVector() * 100.0f;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionQueryParams))
	{
		// UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::CheckFall) OutHit Actor Name : %s"), *OutHit.GetActor()->GetActorNameOrLabel());
		return false;
	}
	
	return true;
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameMode && GameMode->IsRoundStarted() == false) return;

	if (CheckFall())
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::Tick) %s CheckFall True"), *GetActorNameOrLabel());
		GameMode->SetLoseCharacter(this);
	}
	
	// 공격 성공, 방어 성공, 피격, 공격 실패일 경우 앞뒤로, 방어 상태일 경우 좌우로 이동
	/*if (bMove)
	{
		MoveTime += DeltaTime;
		
		 FVector NewLocation = FMath::Lerp(GetActorLocation(), Destination, MoveTime);
		 SetActorLocation(NewLocation);

		SetActorLocation(Destination);
		
		if (MoveTime > 1.0f)
		{
			MoveTime = 0.0f;
			bMove = false;
			bIsMoveVertical = false;
		}
	}*/
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterBase::SetTarget(ACharacterBase* NewTarget)
{
	Target = NewTarget;
	GazeAtTarget();
}

bool ACharacterBase::IsDefence()
{
	return true;
}

float ACharacterBase::GetMainHandAngle()
{
	return 0;
}

float ACharacterBase::GetSubHandAngle()
{
	return 0;
}

bool ACharacterBase::IsAttack()
{
	return true;
}

// 공격 성공
void ACharacterBase::SuccessAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ACharacterBase::SuccessAttack) MoveDistance : %f"), MoveDistance);
	// 앞으로 MoveDistance 만큼 이동
	MoveVertical(MoveDistance);
}

// 방어 성공
void ACharacterBase::SuccessDefence()
{
	// 앞으로 MoveDistance 만큼 이동
	MoveVertical(SuccessDefenceDistance);
}

// 피격
void ACharacterBase::ReceiveDamage()
{
	// 못 막았을 때, 방어 상태가 풀리도록
	bIsDefence = false;
	Weapon->SetDefenceMode(false);

	UE_LOG(LogTemp, Warning, TEXT("ACharacterBase::ReceiveDamage) MoveDistance : %f"), MoveDistance);

	// 뒤로 -MoveDistance 만큼 이동
	MoveVertical(MoveDistance * -1);
	
	if (bIsStun)
	{
		StopStun();
	}
}

// 공격 실패
void ACharacterBase::FailAttack()
{
	// 뒤로 -MoveDistance 만큼 이동
	MoveVertical(SuccessDefenceDistance * -1);
	
	StartStun();
}

void ACharacterBase::MoveVertical(float Distance)
{
	//Destination = GetActorLocation() + GetActorForwardVector() * Distance;
	// bMove = true;
	// bIsMoveVertical = true;

	if (Target) AddActorWorldOffset((Target->GetActorLocation() - GetActorLocation()).GetSafeNormal() * Distance);
}

void ACharacterBase::MoveHorizontal(float SwordAngle)
{
	if (Target == nullptr) return;
	
	float MaxValue = 180.0f;
	float MinValue = 0.0f;
	
	// 검의 벡터 값을 받아서 0-1값으로 정규화
	float NewAngle = (SwordAngle - MinValue) / (MaxValue - MinValue);

	// 정규화된 값을 이용해서 새로운 Distance 도출
	float NewDistance = MoveHorizontalDistance * NewAngle;
	
	FVector ForwardVector = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	AddActorWorldOffset(FRotator(0, 90, 0).RotateVector(ForwardVector) * NewDistance);
	// AddActorWorldOffset(GetActorRightVector() * NewDistance);
	// Destination = GetActorLocation() + GetActorRightVector() * NewDistance;
	// bMove = true;
}

void ACharacterBase::StartStun()
{
	bIsStun = true;

	// 일정 시간 동안 입력 무시
	// 일정 시간 이후 또는 맞으면 원복
	UE_LOG(LogTemp, Warning, TEXT("Start Stun"));

	GetWorld()->GetTimerManager().ClearTimer(StunTimerHandle);
	
	if (StunTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(StunTimerHandle);
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ACharacterBase::StopStun, StunTime, false, StunTime);
}

void ACharacterBase::StopStun()
{
	bIsStun = false;

	UE_LOG(LogTemp, Warning, TEXT("Stop Stun"));
	// 재동작
}

AWeaponBase* ACharacterBase::GetWeapon()
{
	return Weapon;
}

FVector ACharacterBase::GetAttackAngle()
{
	return FVector();
}

void ACharacterBase::GazeAtTarget()
{
}

void ACharacterBase::SetMoveDistance(float NewDistance)
{
	MoveDistance = NewDistance;
}

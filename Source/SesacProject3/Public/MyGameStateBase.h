// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

class ACharacterBase;
class APlayerStart;

UCLASS()
class SESACPROJECT3_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	AMyGameStateBase();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void AddPlayer(ACharacterBase* NewPlayer);
	void SetLoseCharacter(ACharacterBase* NewLoseCharacter);
	
	void MoveToNextRound();
	bool IsRoundStarted() const;
	void StartRound();
	
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	bool bIsRoundStarted;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	float CurrentRoundTime;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	float MaxRoundTime = 15.f;

	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	TArray<int32> WinResultArray;

public:
	// Delegate
	DECLARE_DELEGATE_OneParam(FDele_Single_One_Int, int);
	FDele_Single_One_Int WinResultAdded;
	// Delegate
	DECLARE_DELEGATE_OneParam(FDele_Single_One_Float, float);
	FDele_Single_One_Float RoundTimeChanged;
	// Timer
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	FTimerHandle WaitForPlayerReadyTimer;
};
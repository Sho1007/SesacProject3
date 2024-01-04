// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WidgetComp.h"
#include "Character/PlayerCharacter.h"
#include "Components/WidgetInteractionComponent.h"
#include "EnhancedInputComponent.h"

UWidgetComp::UWidgetComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWidgetComp::BeginPlay()
{
	Super::BeginPlay();

	Player = GetOwner<APlayerCharacter>();
}

void UWidgetComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWidgetComp::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* input)
{
	EnhancedInputComponent->BindAction(input, ETriggerEvent::Triggered, this, &UWidgetComp::MouseLeftClick);
	EnhancedInputComponent->BindAction(input, ETriggerEvent::Completed, this, &UWidgetComp::MouseLeftRelease);
}

void UWidgetComp::MouseLeftClick(const FInputActionValue& value)
{
	Player->LeftPointer->PressPointerKey(EKeys::LeftMouseButton); // �� �̺�Ʈ�� ����Ǹ� LeftMouseButton�� ���� �Ͱ� ���� ȿ���� �ְڴ�.(�̷������� Ű ȿ�� �ο� ����)
}

void UWidgetComp::MouseLeftRelease(const FInputActionValue& value)
{
	Player->LeftPointer->ReleasePointerKey(EKeys::LeftMouseButton);
}

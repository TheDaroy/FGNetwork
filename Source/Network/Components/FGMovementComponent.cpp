// Fill out your copyright notice in the Description page of Project Settings.

#include "FGMovementComponent.h"
#include "GameFramework/Actor.h"
#include "../FGMovementStatics.h"
#include "Engine/World.h"

void UFGMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	FacingRotationCurrent = FQuat::Slerp(FacingRotationCurrent.Quaternion(), FacingRotationTarget.Quaternion(),FacingRotaitonSpeed * DeltaTime).Rotator();
	if (FacingRotationCurrent.Equals(FacingRotationTarget))
	{
		FacingRotationCurrent = FacingRotationTarget;
		SetComponentTickEnabled(false);
	}
}

FFGFrameMovement UFGMovementComponent::CreateFrameMovement() const
{
	return FFGFrameMovement(UpdatedComponent);
}

void UFGMovementComponent::Move(FFGFrameMovement& FrameMovement)
{
	Hit.Reset();

	FVector Delta = GetMovementDelta(FrameMovement);
	MoveUpdatedComponent(Delta, FacingRotationCurrent, true, &Hit);

	if (Hit.bBlockingHit && FVector::DotProduct(FVector::UpVector, Hit.Normal) > 0.0F)
	{
		AccumulatedGravity = 0.0F;
		Delta = GetMovementDelta(FrameMovement);
	}

	SlideAlongSurface(Delta, 1.0F - Hit.Time, Hit.Normal, Hit);

	FrameMovement.Hit = Hit;
	FrameMovement.FinalLocation = UpdatedComponent->GetComponentLocation();
}

void UFGMovementComponent::ApplyGravity()
{
	AccumulatedGravity =+Gravity * GetWorld()->GetDeltaSeconds();
}

void UFGMovementComponent::SetFacingRotation(const FRotator& InFacingRotation, float InRotationSpeed /*= -1.0f*/)
{
	Internal_SetFacingRotation(InFacingRotation,InRotationSpeed);
}

void UFGMovementComponent::SetFacingRotation(const FQuat& InFacingRotation, float InRotationSpeed /*= -1.0f*/)
{
	Internal_SetFacingRotation(InFacingRotation.Rotator(),InRotationSpeed);
}

void UFGMovementComponent::SetFacingDirection(const FVector& InFacingDirection, float InRotationSpeed /*= -1.0f*/)
{
	Internal_SetFacingRotation(InFacingDirection.Rotation(), InRotationSpeed);
}

void UFGMovementComponent::Internal_SetFacingRotation(const FRotator& InFacingRotation, float InRotationSpeed)
{
	FRotator NewRotation = InFacingRotation;
	NewRotation.Roll = 0.0f;
	NewRotation.Pitch = 0.0f;

	FacingRotationTarget = NewRotation;
	if (InRotationSpeed < 0.0f)
	{
		FacingRotationCurrent = NewRotation;
		SetComponentTickEnabled(false);
	}
	else
	{
		SetComponentTickEnabled(true);
	}

}

FVector UFGMovementComponent::GetMovementDelta(const FFGFrameMovement& FrameMovement) const
{
	return FrameMovement.GetMovementDelta() - GetGravityAsVector();
}



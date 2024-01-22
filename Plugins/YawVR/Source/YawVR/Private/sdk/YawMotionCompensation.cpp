// Fill out your copyright notice in the Description page of Project Settings.


#include "YawMotionCompensation.h"

// Sets default values for this component's properties
UYawMotionCompensation::UYawMotionCompensation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UYawMotionCompensation::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	for (int i = 0; i < FrameBufferSize; i++)
	{
		FRotator newRotation = FRotator::ZeroRotator;
		RotationBuffer.Add(newRotation);
	}
}


// Called every frame
void UYawMotionCompensation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UYawMotionCompensation::UpdateFrameBufferAverage(FRotator NewFrame)
{
	if(CurrentFrameBuffer < RotationBuffer.Num())
		RotationBuffer[CurrentFrameBuffer++] = NewFrame;

	if (CurrentFrameBuffer >= FrameBufferSize)
		CurrentFrameBuffer = 0;
}

FRotator UYawMotionCompensation::GetAveragedFrameBuffer()
{
	AverageFrameBuffer = FRotator::ZeroRotator;

	for (int i = 0; i < FrameBufferSize; i++)
	{
		AverageFrameBuffer.Roll += RotationBuffer[i].Roll;
		AverageFrameBuffer.Pitch += RotationBuffer[i].Pitch;
		AverageFrameBuffer.Yaw += RotationBuffer[i].Yaw;
	}

	AverageFrameBuffer.Roll /= FrameBufferSize;
	AverageFrameBuffer.Pitch /= FrameBufferSize;
	AverageFrameBuffer.Yaw /= FrameBufferSize;

	return AverageFrameBuffer;
}

void UYawMotionCompensation::IncrementFrameBufferSize(int32 amount)
{
	FrameBufferSize = FMath::Clamp( (FrameBufferSize + amount), 0, 100);

	RotationBuffer.Empty();

	for (int i = 0; i < FrameBufferSize; i++)
	{
		FRotator newRotation = FRotator::ZeroRotator;
		RotationBuffer.Add(newRotation);
	}
}

float UYawMotionCompensation::ProcessDeviceYaw(float DeviceYaw)
{
	DeviceYaw *= -1.f;

	if (FMath::Abs(SmoothedYaw) - FMath::Abs(DeviceYaw) > 100.f)
	{
		SmoothedYaw = FMath::FInterpTo(SmoothedYaw, DeviceYaw, GetWorld()->DeltaTimeSeconds, 10.f);
	}
	else 
	{
		SmoothedYaw = DeviceYaw;
	}

	return SmoothedYaw;
}

FVector UYawMotionCompensation::CalculateTranslationOffsets(FRotator SoftwareMotionValues, float CalibratedYaw)
{

//X = Cx + (r * cosine(angle))  
//Y = Cy + (r * sine(angle))

//(0, 180)(-180, 0)
	CurrentMotionBaseYawAngle = NormalizeDeviceYaw(SoftwareMotionValues.Yaw - CalibratedYaw);

	CurrentYawOffsetSurge = MaxYawRadius * -FMath::Cos(CurrentMotionBaseYawAngle);
	CurrentYawOffsetSway = MaxYawRadius * FMath::Sin(CurrentMotionBaseYawAngle);

	CurrentPRSurgeOffset = (-(SoftwareMotionValues.Pitch / MaxMotionBasePitchF) * MaxMotionBaseTranslations.X);
	CurrentPRSwayOffset = (-(SoftwareMotionValues.Roll / MaxMotionBaseRoll) * MaxMotionBaseTranslations.Y);
	CurrentPRHeaveOffset = (-(SoftwareMotionValues.Pitch / MaxMotionBasePitchF) * MaxMotionBaseTranslations.Z);

	CurrentTranslationOffsets.X = CurrentPRSurgeOffset + CurrentYawOffsetSurge;
	CurrentTranslationOffsets.Y = CurrentPRSwayOffset + CurrentYawOffsetSway;
	CurrentTranslationOffsets.Z = CurrentPRHeaveOffset;

	return CurrentTranslationOffsets;
}

//Normalizes Yaw [0, 360]
float UYawMotionCompensation::NormalizeDeviceYaw(float CurrentYaw)
{
	if (CurrentYaw < 0)
		CurrentYaw = 180 + (180 - FMath::Abs(CurrentYaw));

	CurrentYaw = FMath::DegreesToRadians(CurrentYaw);

	return CurrentYaw;
}
// Fill out your copyright notice in the Description page of Project Settings.
/*
Author - Michael Arevalo (Mikeusesemail@gmail.com)
This is an experimental solution for motion compensation using just code. This is not ready for release to consumers
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "YawMotionCompensation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UYawMotionCompensation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UYawMotionCompensation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float SmoothedYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float MaxMotionBaseRoll = 26;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float MaxMotionBasePitchF = 18;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float MaxMotionBasePitchB = 28;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float CurrentPRSwayOffset = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float CurrentPRSurgeOffset= 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Set Up")
	float CurrentPRHeaveOffset = 0;

	float MaxYawXTranslation = -46.5f;

	float MaxYawYTranslation = 27;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
	float MaxYawRadius = 19;

	UPROPERTY(VisibleAnywhere, Category = "Processing")
	float CurrentYawOffsetSurge = 0;

	UPROPERTY(VisibleAnywhere, Category = "Processing")
	float CurrentYawOffsetSway = 0;

	UPROPERTY(VisibleAnywhere, Category = "Processing")
	float CurrentMotionBaseYawAngle = 0;

	FVector MaxMotionBaseTranslations = FVector(-12.3f, 12.3f, -5.f);

	UPROPERTY(VisibleAnywhere, Category = "Processing")
	FVector CurrentTranslationOffsets = FVector( 0, 0, 0);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Processing")
	int32 FrameBufferSize = 5;

	int32 CurrentFrameBuffer = 0;

	FRotator AverageFrameBuffer;

	UPROPERTY(VisibleAnywhere, Category = "Processing")
	TArray<FRotator> RotationBuffer;

	UFUNCTION(BlueprintCallable, Category = "Processing")
	float ProcessDeviceYaw(float DeviceYaw);

	UFUNCTION(BlueprintCallable, Category = "Processing")
	FVector CalculateTranslationOffsets(FRotator SoftwareMotionValues, float CalibratedYaw);

	UFUNCTION(BlueprintCallable, Category = "Processing")
	FRotator GetAveragedFrameBuffer();

	UFUNCTION(BlueprintCallable, Category = "Processing")
	void IncrementFrameBufferSize(int32 amount);

	UFUNCTION(BlueprintCallable, Category = "Processing")
	void UpdateFrameBufferAverage(FRotator NewFrame);

	float NormalizeDeviceYaw(float CurrentYaw);
		
};

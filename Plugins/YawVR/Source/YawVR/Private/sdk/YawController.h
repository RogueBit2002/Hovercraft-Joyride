#pragma once

#include "ActionBus.h"

#include "Enums.h"
#include "YawDevice.h"
#include "RotationLimits.h"

#include <SocketSubsystem.h>
#include <IPAddress.h>
#include "Networking.h"

#include "Enums.h"
#include "RotationLimits.h"
#include "Commands.h"

#include "YawTCPClient.h"
#include "YawUDPClient.h"

#include "YawController.generated.h"

USTRUCT(BlueprintType)
struct FBuzzer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int RightBuzzer = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int CenterBuzzer = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int LeftBuzzer = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int BuzzerHz = 0;
};


// abstract classes
class YawControllerDelegate
{
public:
	virtual void ControllerStateChanged(ControllerState state) = 0;
	virtual void DidFoundDevice(TSharedPtr<YawDevice> device) = 0;
	virtual void DidDisconnectFrom(TSharedPtr<YawDevice> device) = 0;
	virtual void YawLimitDidChange(int currentLimit) = 0;
	virtual void TiltLimitsDidChange(int pitchFrontLimit, int pitchBackLimit, int rollLimit) = 0;
};

class YawControllerType
{
public:
	YawControllerType() : tiltLimits(30, 30, 30) {};

    //Properties 
    ControllerState State;

	TSharedPtr<YawDevice> device;
	YawControllerDelegate* ControllerDelegate = NULL;
	int connectionTimeout;
	int connectionTimeoutCurrent;

	

    //Motion related properties
    TiltLimits tiltLimits;
    float YawLimit;

    //Game related setters
	virtual void SetGameName(FString gameName) = 0;

    //Methods triggering delegate functions
	virtual void DiscoverDevices(int onPort) = 0;
	virtual void SetTiltLimits(int pitchFrontLimit, int pitchBackLimit, int rollLimit) = 0;
	virtual void SetYawLimit(int yawLimit) = 0;

    //Methods with success/error action callbacks
	virtual void ConnectToDevice(TSharedPtr<YawDevice> yawDevice) = 0;
	virtual void StartDevice() = 0;
	virtual void StopDevice() = 0;
	virtual void DisconnectFromDevice() = 0;

    //Setters related to motion data processing
	virtual void SetMotionSampleSize(int size) = 0;
};


UCLASS(hidecategories = (Object, LOD, Physics, Collision), editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Utility)
class UYawController : public USceneComponent, public YawControllerType, public YawTCPClientDelegate, public YawUDPClientDelegate, public YawControllerDelegate
{

	GENERATED_UCLASS_BODY()

public:
	//UYawController() {};

    //MARK: - Serializable private fields

	//DELETE these after all references ahve been reworked to the component transform
    //USceneComponent *referenceComponent;
	//Rigidbody referenceRigidbody = NULL;

    FString gameName;
    int udpClientPort;

	int motionSampleSize;

    //MARK: - Properties 

	YawControllerDelegate* ControllerDelegate = NULL;

	YawTCPClient* tcpCLient = NULL;
	YawUDPClient* udpClient = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SetUp")
	ControllerState state;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SetUp")
	FRotator DeviceActualRotation;

	FVector CustomRotation = FVector(0,0,0);

	FBuzzer CurrentBuzzer;

	int discoveryPort;

	float yawLimit;

    //MARK: - Lifecycle methods

    virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void BeginDestroy() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    //MARK: - Game related setters

    void SetGameName(FString gameNameS);

    //MARK: - Methods triggering delegate functions

    void DiscoverDevices(int onPort);

	UFUNCTION(BlueprintCallable, Category = SetUp)
    void SetYawLimit(int yawLimitI);

	UFUNCTION(BlueprintCallable, Category = SetUp)
    void SetTiltLimits(int pitchForwardLimit, int pitchBackwardLimit, int rollLimit);

    //MARK: - Methods with success/error action callbacks

    void ConnectToDevice(TSharedPtr<YawDevice> yawDevice) override;

    void StartDevice() override;

    void StopDevice() override;

	UFUNCTION(BlueprintCallable, Category = SetUp)
    void DisconnectFromDevice() override;

    // MARK: - Delegate methods

    void DidRecieveUDPMessage(FString message, const FIPv4Endpoint& remoteEndPoint) override;

    void DidRecieveTCPMessage(TArray<uint8> data) override;

    void DidLostServerConnection() override;


	//MARK: - YawControllerDelegate overrides

	void ControllerStateChanged(ControllerState stateCSs) override;
	void DidFoundDevice(TSharedPtr<YawDevice> deviceYD) override ;
	void DidDisconnectFrom(TSharedPtr<YawDevice> deviceYD) override;
	void YawLimitDidChange(int currentLimit) override;
	void TiltLimitsDidChange(int pitchFrontLimit, int pitchBackLimit, int rollLimit) override;

    void SetMotionSampleSize(int size);

    void SendMotionData();

    //MARK: - UDP command sender functions

    void SendRotation(FVector rotation);

    //MARK: - Helper functions

    void SetState(ControllerState newState);

    //IEnumerator ResponseTimeout(Action/*<string>*/ onError);

    float SignedForm(float angle);

    float UnsignedForm(float angle);

	//Use this function whenever you would like to update motion values
	UFUNCTION(BlueprintCallable, Category = SetUp)
	void UpdateMotionValues(FRotator NewRotation);

	//Use this function whenever you would like to update buzzer values
	UFUNCTION(BlueprintCallable, Category = SetUp)
	void UpdateBuzzerValues(FBuzzer NewBuzzerValues);

	void SetUpValues();
};



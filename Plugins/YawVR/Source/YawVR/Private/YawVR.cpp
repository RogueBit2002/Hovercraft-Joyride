#include "YawVR.h"

#include "Modules/ModuleManager.h"
#include "Containers/Ticker.h"

#include "sdk/YawUDPClient.h"
#include "sdk/YawController.h"

#include "IYawVR.h"



class FYawVR : public IYawVR
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;

	bool Tick(float DeltaTime);
	void Debug();

	//YawUDPClient* udpClient;

	//TSharedPtr<UYawController> yawController;
};

IMPLEMENT_MODULE( FYawVR, YawVR )

void FYawVR::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

	//TickDelegate = FTickerDelegate::CreateRaw(this, &FYawVR::Tick);
	//TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate, 2);

	//udpClient = new YawUDPClient(50060);
	//udpClient->udpDelegate = this;
	//udpClient->StartListening();

	//yawController = TSharedPtr<UYawController>(new UYawController());
}


void FYawVR::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	//yawController = NULL;
}

bool FYawVR::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("This is an on screen message!"));


	// https://wiki.unrealengine.com/Logs,_Printing_Messages_To_Yourself_During_Runtime
	//UE_LOG(LogTemp, Warning, TEXT("Test UE_LOG Warning"));

	
	//UE_LOG(LogTemp, Warning, TEXT("YAW_CALLING 8"));

	//FString Message = "YAW_CALLING";
	//FTCHARToUTF8 Converted(*Message);

	//udpClient->SendBroadcast(50010, (uint8*)Converted.Get(), Converted.Length()); // https://docs.unrealengine.com/en-US/Programming/UnrealArchitecture/StringHandling/CharacterEncoding
	//udpClient->SendBroadcast(50010, (uint8*)TCHAR_TO_UTF8(Message.GetCharArray().GetData())); // https://docs.unrealengine.com/en-US/Programming/UnrealArchitecture/StringHandling/CharacterEncoding

	return true;
}

void FYawVR::Debug()
{
	/*
	rotationText.text = "Rotation: " + YawController.Instance().ReferenceRotation.ToString();
	velocityText.text = "Velocity: " + YawController.Instance().ReferenceVelocity.ToString();
	accelerationText.text = "Acceleration: " + YawController.Instance().ReferenceAcceleration.ToString();
	turnAngleText.text = "Turn angle(d/s): " + YawController.Instance().ReferenceTurnAngle.ToString();
	lateralForceText.text = "Lateral force: " + YawController.Instance().ReferenceLateralForce.ToString();
	*/
}



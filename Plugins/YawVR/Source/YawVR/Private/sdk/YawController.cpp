#include "YawController.h"
#include "YawVR.h"


//MARK: - Lifecycle methods

UYawController::UYawController(const class FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	UE_LOG(LogTemp, Log, TEXT("UYawController Constructor"));


	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetUpValues();

}

void UYawController::SetUpValues()
{
	udpClientPort = 50060;
	discoveryPort = 50010;

	motionSampleSize = 5;

	yawLimit = 0;

	device = NULL;
	connectionTimeout = 500;
	connectionTimeoutCurrent = 0;

	ControllerDelegate = NULL;

	tcpCLient = NULL;
	udpClient = NULL;

	SetMotionSampleSize(motionSampleSize);
}

void UYawController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UYawController BeginPlay"));

	//SetUpValues();

	state = ControllerState::Initial;

	ControllerDelegate = this;


	tcpCLient = new YawTCPClient();
	tcpCLient->tcpDelegate = this;

	/*
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this]
	{
		if (IsValid(this) && tcpCLient.IsValid()) {
			tcpCLient->Receive();
		}
	});
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 0.01f, true);
	*/


	udpClient = new YawUDPClient(udpClientPort);
	udpClient->udpDelegate = this;
	udpClient->StartListening();

	//GEngine->AddOnScreenDebugMessage(-1, 1000.f, FColor::Red, "Begin Play");
}


void UYawController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Log, TEXT("UYawController EndPlay"));

	//If our application terminates, sending Exit command to simulator if needed 
	if (IsValid(this) && state != ControllerState::Initial && state != ControllerState::Disconnecting && device.IsValid())
	{
		DisconnectFromDevice();
	}

	//Closing tcp & udp clients
	if (tcpCLient != NULL) {
		UE_LOG(LogTemp, Log, TEXT("Closing tcp client connection"));
		tcpCLient->CloseConnection();
		//delete tcpCLient;
	}
	if (udpClient != NULL) {
		UE_LOG(LogTemp, Log, TEXT("Closing udp client connection"));
		udpClient->StopListening();
		//delete udpClient;
	}
}
void UYawController::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogTemp, Log, TEXT("UYawController BeginDestroy"));

	//If our application terminates, sending Exit command to simulator if needed 
	if (IsValid(this) && state != ControllerState::Initial && state != ControllerState::Disconnecting && device.IsValid())
	{
		DisconnectFromDevice();
	}

	//Closing tcp & udp clients
	if (tcpCLient != NULL) {
		tcpCLient->CloseConnection();
		//delete tcpCLient;
	}
	if (udpClient != NULL) {
		udpClient->StopListening();
		//delete udpClient;
	}

	ControllerDelegate = NULL;
}

void UYawController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (tcpCLient != NULL) {
		tcpCLient->Receive();
	}

	ESocketErrors ErrorCode = ISocketSubsystem::Get()->GetLastErrorCode();

	switch (state) {
		case ControllerState::Connected:
			UE_LOG(LogTemp, Log, TEXT("Connected"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Connected");
			break;
		case ControllerState::Connecting:
			UE_LOG(LogTemp, Log, TEXT("Connecting"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Connecting");

			//UE_LOG(LogTemp, Log, TEXT("Connecting %i"), connectionTimeoutCurrent);
			if (tcpCLient && tcpCLient->Socket) {
				switch (tcpCLient->Socket->GetConnectionState())
				{
				case ESocketConnectionState::SCS_Connected:
					UE_LOG(LogTemp, Log, TEXT("Socket SCS_Connected %i"), int32(ErrorCode));
					if (connectionTimeoutCurrent % 100 == 0) {
						tcpCLient->Send(Commands::CHECK_IN(udpClientPort, gameName));
					}
					break;
				case ESocketConnectionState::SCS_ConnectionError:
					UE_LOG(LogTemp, Log, TEXT("Socket SCS_ConnectionError %i"), int32(ErrorCode));
					break;
				case ESocketConnectionState::SCS_NotConnected:
					UE_LOG(LogTemp, Log, TEXT("Socket SCS_NotConnected %i"), int32(ErrorCode));
					break;
				}
			}

			connectionTimeoutCurrent--;
			if (connectionTimeoutCurrent <= 0) {
				DisconnectFromDevice();
			}
			break;
		case ControllerState::Disconnecting:
			UE_LOG(LogTemp, Log, TEXT("Disconnecting"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Disconnecting");

			DiscoverDevices(50010);
			break;
		case ControllerState::Initial:
			UE_LOG(LogTemp, Log, TEXT("Initial"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Initial");

			DiscoverDevices(50010);
			break;
		case ControllerState::Started:
			//UE_LOG(LogTemp, Log, TEXT("MA: Started"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Started");

			SendMotionData();
			break;
		case ControllerState::Starting:
			UE_LOG(LogTemp, Log, TEXT("Starting"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Starting");
			break;
		case ControllerState::Stopping:
			UE_LOG(LogTemp, Log, TEXT("Stopping"));
			//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, "Stopping");
			break;
	}
}

//MARK: - Game related setters

void UYawController::SetGameName(FString gameNameS)
{
	this->gameName = gameNameS;
}

//MARK: - Methods triggering delegate functions

void UYawController::DiscoverDevices(int onPort)
{
	//UE_LOG(LogTemp, Log, TEXT("MA: Discovering devices"));

    //Save a reference to port, which will be used in creating yawDevices when discovery responses arrive
    //We have to use their listening port (this) - not from which it sends response
    discoveryPort = onPort;

    //Send the discovery broadcast
	udpClient->SendBroadcast(onPort, Commands::DEVICE_DISCOVERY);
}

void UYawController::SetYawLimit(int yawLimitI)
{
    //If we are connected, send a request to set yaw limit
    if (state != ControllerState::Initial && state != ControllerState::Disconnecting)
    {
		tcpCLient->Send(Commands::SET_YAW_LIMIT(yawLimitI));
    }
}

void UYawController::SetTiltLimits(int pitchForwardLimit, int pitchBackwardLimit, int rollLimit)
{
    //If we are connected, send a request to set tilt limits
    if (state != ControllerState::Initial && state != ControllerState::Disconnecting)
    {
		tcpCLient->Send(Commands::SET_TILT_LIMITS(pitchForwardLimit, pitchBackwardLimit, rollLimit));
    }
}

//MARK: - Methods with success/error action callbacks

void UYawController::ConnectToDevice(TSharedPtr<YawDevice> yawDevice)
{
	if (yawDevice->status != DeviceStatus::Available)
	{
		UE_LOG(LogTemp, Log, TEXT("device unavailable"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ConnectToDevice"));

	if (state != ControllerState::Initial)
	{
		//If we are already connected to a device, disconnect from it, then connect to new one
		DisconnectFromDevice();
	}

	SetState(ControllerState::Connecting);
	connectionTimeoutCurrent = connectionTimeout;

	//Start connecting to simulator's tcp server
	tcpCLient->Initialize(yawDevice->ipAddress->ToString(), yawDevice->tcpPort);

	//Set connected device to this device 
	device = yawDevice;

	//Send CHECK_IN command
	tcpCLient->Send(Commands::CHECK_IN(udpClientPort, gameName));
}

void UYawController::StartDevice()
{
    if (state == ControllerState::Connected)
    {
		/*
        //Set START command callbacks and start command timeout
        callBacks.startSuccess = onSuccess;
        callBacks.startError = onError;
        callbackTimeouts.startTimeout = StartCoroutine(ResponseTimeout(onError));
		*/
        //Send START command
		tcpCLient->Send(Commands::START);
        //Set state to starting
        SetState(ControllerState::Starting);
    } else {
        // onError("Attempted to start device when device has not been in connected ready state");
    }
}

void UYawController::StopDevice()
{
    if (state == ControllerState::Started)
    {
		/*
        //Set STOP command callbacks and start command timeout
        callBacks.stopSuccess = onSuccess;
        callBacks.stopError = onError;
        callbackTimeouts.stopTimeout = StartCoroutine(ResponseTimeout(onError));
		*/
        //Send STOP command
		tcpCLient->Send(Commands::STOP);
        //Set state to stopping
        SetState(ControllerState::Stopping);
    } else {
        //onError("Attempted to stop simulator when simulator had not been in started state");
    }
}

void UYawController::DisconnectFromDevice()
{
	UE_LOG(LogTemp, Log, TEXT("DisconnectFromDevice"));
    if (state != ControllerState::Initial)
    {
		/*
		//Set EXIT command callbacks and start command timeout
        callBacks.exitSuccess = onSuccess;
        callBacks.exitError = onError;
        callbackTimeouts.exitTimeout = StartCoroutine(ResponseTimeout((error) => {
            //If we reach timeout without server response, set state back to initial
            //This way we reach disconnected and ready state anyway
            SetState(ControllerState::Initial);
            if (onError != null) {
                onError(error);
            }
        }));
		*/
        //Send EXIT command
		tcpCLient->Send(Commands::EXIT);

        //Set state to disconnecting
        SetState(ControllerState::Disconnecting);
    }
    else
    {
        //onError("Attempted to disconnect when no device was connected");
    }
}

// MARK: - Delegate methods

void UYawController::DidRecieveUDPMessage(FString message, const FIPv4Endpoint& remoteEndPoint)
{
	//UE_LOG(LogTemp, Log, TEXT("MA: Got: %s"), *message);
    if (
			message.StartsWith("SY[")  && 
			state != ControllerState::Initial &&
			//Only accept position report from the connected simulator ip address
			(device->ipAddress->ToString() == remoteEndPoint.Address.ToString())
        )
    {

        //We recieved a position report from the connected simulator
        //Extracting rotation values from the message if it is valid
		TArray<FString> messageParts = {};
		const TCHAR* Delims[] = { TEXT("["), TEXT("]")};
		message.ParseIntoArray(messageParts, Delims, 2);
        //if (messageParts.Num() != 6) return;
        float yaw = FCString::Atof(*(messageParts[3])),
			pitch = FCString::Atof(*(messageParts[5])),
			roll = FCString::Atof(*(messageParts[1]));
		// ?? Y[7890]P[76567]R[76876]
		// Y 7890 P 76567 R 76876
            //Set device's actual position
		FVector eulerAnglesVector = FVector(pitch, yaw, roll);
        device->ActualPosition = eulerAnglesVector;
		
		DeviceActualRotation.Roll = pitch;
		DeviceActualRotation.Pitch = yaw;
		DeviceActualRotation.Yaw = roll;

		//UE_LOG(LogTemp, Log, TEXT("MA: Received values"));
    }
    else if (message.Contains("YAWDEVICE"))
    {
        //We recieved a device discovery answer message
        //example device discovery answer: "YAWDEVICE;MacAddrId;MyDeviceName;" + tcpServerPort + (state == DeviceState.Available ? ";OK" : ";RESERVED");
		TArray<FString> messageParts = {};
		message.ParseIntoArray(messageParts, TEXT(";"));
		FIPv4Address* ip = new FIPv4Address(remoteEndPoint.Address);
		auto udp = discoveryPort;
        int tcp = FCString::Atoi(*(messageParts[3]));

		UE_LOG(LogTemp, Log, TEXT("MA: FOUND YAWDEVICE %i %s"), messageParts.Num(), *message);
		
        if (messageParts.Num() == 5)// && int.TryParse(messageParts[3], out tcp))
        {
            DeviceStatus status = messageParts[4].Contains("AVAILABLE") ? DeviceStatus::Available : DeviceStatus::Reserved;
			TSharedPtr<YawDevice> yawDevice = TSharedPtr<YawDevice>(new YawDevice(ip, tcp, udp, messageParts[1], messageParts[2], status));
            
			ControllerDelegate->DidFoundDevice(yawDevice);
        }
    }
}

void UYawController::DidRecieveTCPMessage(TArray<uint8> data)
{
    //data.Num() can't be 0 - YawTcpClient would not dispatch it
    //Read command id from the array
	uint8 commandId = data[0];

	UE_LOG(LogTemp, Log, TEXT("commandId %i"), commandId);

    switch (commandId)
    {
        case CommandIds::CHECK_IN_ANS:
            if (state == ControllerState::Connecting) {
				FString message = FString(UTF8_TO_TCHAR(data.GetData() + 1));
				UE_LOG(LogTemp, Log, TEXT("MA: message %s"), *message);

                if (message.Contains("AVAILABLE"))
                {
                    //Simulator is available, we have succesfully checked in, set state to connected
					udpClient->SetRemoteEndPoint(device->ipAddress->ToString(), device->udpPort);
                    SetState(ControllerState::Connected);
                    //Call success callback
                    //callBacks.connectingSuccess();
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Connected to Device %s" + (device->ipAddress->ToString())) );

					StartDevice();
                }
                else
                {
                    //Simulator is reserved, setting state back to initial
					TArray<FString> messageParts = {};
					const TCHAR* Delims[] = { TEXT(";") };
					message.ParseIntoArray(messageParts, Delims, 1);
					if (messageParts.Num() != 3) return;

					FString reservingGameName = messageParts[1];
					FString reservingIp = messageParts[2];
                    SetState(ControllerState::Initial);

                    //Call error callback
                    //callBacks.connectingError("Device is in use from: " + reservingIp + " with game: " + reservingGameName);
					UE_LOG(LogTemp, Log, TEXT("MA: Device is in use from: %s with game: %s"), *reservingIp, *reservingGameName);
                }
            }
            break;

        case CommandIds::START:
            if (state == ControllerState::Starting)
            {
                //Set state to started
                SetState(ControllerState::Started);
                //Call success callback
                //callBacks.startSuccess();
            } 
            break;

        case CommandIds::STOP:
            if (state != ControllerState::Initial && state != ControllerState::Disconnecting)
            {
                //Set state back to connected
                SetState(ControllerState::Connected);
                //Call success callback
                //callBacks.stopSuccess();
            } 
            break;

        case CommandIds::EXIT:
            //Whenever we got an exit command from connected simulator, we close connection, not only if we invoked that
            SetState(ControllerState::Initial);
            //Call success callback - if we have one
            //callBacks.exitSuccess();
			UE_LOG(LogTemp, Log, TEXT("MA: Device Exit"));
            break;

        case CommandIds::SET_YAW_LIMIT:
            if (data.Num() == 5) {
                int limit = Commands::ByteArrayToInt(data, 1);
				this->yawLimit = limit;
				ControllerDelegate->YawLimitDidChange(limit);
            }
            break;
        case CommandIds::SET_TILT_LIMITS:
            if (data.Num() == 13)
            {
                int pitchForwardLimit = Commands::ByteArrayToInt(data, 1);
                int pitchBackwardLimit = Commands::ByteArrayToInt(data, 5);
                int rollLimit = Commands::ByteArrayToInt(data, 9);

                tiltLimits.pitchForward = pitchForwardLimit;
                tiltLimits.pitchBackward = pitchBackwardLimit;
                tiltLimits.roll = rollLimit;

				ControllerDelegate->TiltLimitsDidChange(pitchForwardLimit, pitchBackwardLimit, rollLimit);
            }
            break;
        default:
            break;
    }
}

void UYawController::DidLostServerConnection()
{
	UE_LOG(LogTemp, Log, TEXT("TCP Client have disconnected"));
	ControllerDelegate->DidDisconnectFrom(device);
    SetState(ControllerState::Initial);
}

//MARK: - YawControllerDelegate overrides

void UYawController::ControllerStateChanged(ControllerState stateCS) {
	
}

void UYawController::DidFoundDevice(TSharedPtr<YawDevice> deviceYD) {
	if (state == ControllerState::Initial || state == ControllerState::Disconnecting) {
		UE_LOG(LogTemp, Display, TEXT("DidFoundDevice"));
		this->ConnectToDevice(deviceYD);
	}
}

void UYawController::DidDisconnectFrom(TSharedPtr<YawDevice> deviceYD) {
	UE_LOG(LogTemp, Error, TEXT("DidDisconnectFrom"));
}

void UYawController::YawLimitDidChange(int currentLimit) {
	UE_LOG(LogTemp, Error, TEXT("YawLimitDidChange"));
}

void UYawController::TiltLimitsDidChange(int pitchFrontLimit, int pitchBackLimit, int rollLimit) {
	UE_LOG(LogTemp, Error, TEXT("TiltLimitsDidChange"));
}


void UYawController::SetMotionSampleSize(int size)
{
    if (size >= 1) {
        motionSampleSize = size;
    }
}

void UYawController::SendMotionData()
{
    if (!device.IsValid() || udpClient == NULL) return;

    //Calculate rotation according to reference motion type - use signed angle form for calculations
    float x = 0, y = 0, z = 0;

	x = SignedForm(CustomRotation.X);
	y = SignedForm(CustomRotation.Y);
	z = SignedForm(CustomRotation.Z);

    //Convert back to unsigned form, and apply absolute limits to be compatible to our UnsignedForm(:float) function
	x = UnsignedForm(x);//FMath::Clamp(x, -90.0f, 90.0f));
	y = UnsignedForm(y);//FMath::Clamp(y, -180.0f, 180.0f));
	z = UnsignedForm(z);///FMath::Clamp(z, -90.0f, 90.0f));

    //Send the calculate rotations, SendRotation(:Vector3) will apply limits
    SendRotation(FVector(x, y, z));
}

//MARK: - UDP command sender functions

void UYawController::SendRotation(FVector rotation)
{
    float x, y, z;
    if (yawLimit > 0)
    {
        y = UnsignedForm(FMath::Clamp(SignedForm(rotation.Y), -YawLimit, YawLimit));
    }
    else
    {
        y = rotation.Y;
    }

    x = UnsignedForm(FMath::Clamp(SignedForm(rotation.X), -tiltLimits.pitchBackward, tiltLimits.pitchForward));
    z = UnsignedForm(FMath::Clamp(SignedForm(rotation.Z), -tiltLimits.roll, tiltLimits.roll));

	udpClient->Send(Commands::SET_POSITION(y, x, z, CurrentBuzzer.RightBuzzer, CurrentBuzzer.CenterBuzzer, CurrentBuzzer.LeftBuzzer, CurrentBuzzer.BuzzerHz));
}


//MARK: - Helper functions
void UYawController::SetState(ControllerState newState)
{
    state = newState;
    if (newState == ControllerState::Initial) {
        device = NULL;
        if (tcpCLient->connected) {
			tcpCLient->CloseConnection();
        }
    }

	ControllerDelegate->ControllerStateChanged(newState);
}

//IEnumerator UYawController::ResponseTimeout(Action/*<string>*/ onError)
//{
//    if (onError == null) yield break;
//    yield return new WaitForSeconds(10f);
//    //onError("Command timeout");
//}

float UYawController::SignedForm(float angle) {
    return angle >= 180 ? angle - 360 : angle;
}

float UYawController::UnsignedForm(float angle) {
    return angle < 0 ? 360 + angle : angle;
}

//Mike - Correcting from Unreal rotation coordinates
void UYawController::UpdateMotionValues(FRotator NewRotation)
{
	//Yaw VR values: x = pitch, y = yaw, z = roll
	CustomRotation.X = -NewRotation.Pitch;
	CustomRotation.Y = NewRotation.Yaw;
	CustomRotation.Z = -NewRotation.Roll;
}

void UYawController::UpdateBuzzerValues(FBuzzer NewBuzzerValues)
{
	CurrentBuzzer = NewBuzzerValues;
}

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <SocketSubsystem.h>
#include "Interfaces/IPv4/IPv4Address.h"
#include <IPAddress.h>

#include "Enums.h"

using std::string;

class YawDevice
{
public:
	FIPv4Address* ipAddress;

    int tcpPort;

    int udpPort;
	FString id;
	FString name;

    DeviceStatus status;

    FVector ActualPosition = FVector();

    YawDevice(FIPv4Address* ipAddress, int tcpPort, int udpPort, FString id, FString name, DeviceStatus status);

    void SetStatus(DeviceStatus statusDS);
};
#include "YawDevice.h"
#include "YawVR.h"

#include <SocketSubsystem.h>
#include <IPAddress.h>

#include "Enums.h"



YawDevice::YawDevice(FIPv4Address* ipAddress, int tcpPort, int udpPort, FString id, FString name, DeviceStatus status) :
    ipAddress(ipAddress), tcpPort(tcpPort), udpPort(udpPort), id(id), name(name), status(status)
{

}

void YawDevice::SetStatus(DeviceStatus statusDS)
{
	this->status = statusDS;
}
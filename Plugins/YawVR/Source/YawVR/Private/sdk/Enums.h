#pragma once

enum class  DeviceStatus
{
	Available, Reserved, Unknown
};

enum class  Result
{
	Success, Error
};

UENUM(BlueprintType)
enum class ControllerState : uint8
{
	Initial, Connecting, Connected, Starting, Started, Stopping, Disconnecting
};
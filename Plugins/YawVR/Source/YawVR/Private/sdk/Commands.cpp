#include "Commands.h"
#include "YawVR.h"

#include "Algo/Reverse.h"

#include "Commands.h"

const TArray<uint8> Commands::START({ CommandIds::START });
const TArray<uint8> Commands::STOP ({ CommandIds::STOP  });
const TArray<uint8> Commands::EXIT ({ CommandIds::EXIT  });

const TArray<uint8> Commands::DEVICE_DISCOVERY(FStringToBytes("YAW_CALLING"));



//example: "Y[000.00]P[359.99]R[180.00]"; - there is no 360.00, just 000.00
TArray<uint8> Commands::SET_POSITION(float yaw, float pitch, float roll, int rightBuzzer, int centerBuzzer, int leftBuzzer, int buzzerHz)
{
    FString message = "Y[" + FormatRotation(yaw) + "]P[" + FormatRotation(pitch) + "]R[" + FormatRotation(roll) + "]";
	FString message2 = "V[" + FormatInt(rightBuzzer) + "," + FormatInt(centerBuzzer) + "," + FormatInt(leftBuzzer) + "," + FormatInt(buzzerHz) + "]";

	message += message2;

	return FStringToBytes(message);
}

//TCP

TArray<uint8> Commands::CHECK_IN(int udpListeningPort, FString gameName)
{
	TArray<uint8> message;
	message.Add(CommandIds::CHECK_IN);

	message.Append(IntToByteArray(udpListeningPort));
	message.Append(FStringToBytes(gameName));

    return message;
}

TArray<uint8> Commands::SET_TILT_LIMITS(int pitchFrontMax, int pitchBackMax, int rollMax)
{
	TArray<uint8> message;
	message.Add(CommandIds::SET_TILT_LIMITS);

	message.Append(IntToByteArray(pitchFrontMax));
	message.Append(IntToByteArray(pitchBackMax));
	message.Append(IntToByteArray(rollMax));


	return message;
}

TArray<uint8> Commands::SET_YAW_LIMIT(int yawMax)
{
	TArray<uint8> message;
	message.Add(CommandIds::SET_YAW_LIMIT);

	message.Append(IntToByteArray(yawMax));


	return message;
}


//MARK: - Helper functions

FString Commands::FormatRotation(float f)
{
    float i = (float)((int)(f * 100)) / (float)100.0;
    while (i < 0) i += 360;
    while (i >= 360) i -= 360;
	FString s = FString::SanitizeFloat(i);
    if (i < 10)
    {
        s = "00" + s;
    }
    else if (i < 100)
    {
        s = "0" + s;
    }
    if (s.Len() == 5) s = s + "0";
    if (s.Len() == 4) s = s + "00";
    if (s.Len() == 3) s = s + ".00";
    return s;
}

FString Commands::FormatInt(int i)
{
	FString s = FString::FromInt(FMath::Clamp(i, 0, 100));
	return s;
}

TArray<uint8> Commands::IntToByteArray(int intValue)
{
	uint8 const *bytesRaw = reinterpret_cast<uint8 const *>(&intValue);
	TArray<uint8> bytes = TArray<uint8>(bytesRaw, sizeof(int));
	if (FGenericPlatformProperties::IsLittleEndian())
	{
		Algo::Reverse(bytes);
	}

    return bytes;
}

TArray<uint8> Commands::FloatToByteArray(float floatValue)
{
	uint8 const *bytesRaw = reinterpret_cast<uint8 const *>(&floatValue);
	TArray<uint8> bytes = TArray<uint8>(bytesRaw, sizeof(float));
	if (FGenericPlatformProperties::IsLittleEndian())
	{
		Algo::Reverse(bytes);
	}

	return bytes;
}

int Commands::ByteArrayToInt(TArray<uint8> intBytes, int startIndex)
{
	int *integer = reinterpret_cast<int *>(&intBytes[startIndex]);

    return *integer;
}

float Commands::ByteArrayToFloat(TArray<uint8> floatBytes, int startIndex)
{
	float *floatNumber = reinterpret_cast<float *>(&floatBytes[startIndex]);

	return *floatNumber;
}

TArray<uint8> Commands::FStringToBytes(FString Message)
{
	FTCHARToUTF8 Converted(*Message);
	uint8* data = (uint8*)Converted.Get();

	return TArray<uint8>(data, Converted.Length());
};

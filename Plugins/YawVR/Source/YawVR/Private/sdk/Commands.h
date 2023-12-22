#pragma once

/*Communication between the game and the simulator:
    (Commands can be created easily by calling static functions on Commands class)

    UDP messages:
    Every udp command is ascii encoded string sent as byte array

    TCP messages:
    Every tcp command begins with the byte identifier of the given command, 
    followed by the command parameters.
    Integer and float parameters are converted into 4 bytes (sent in big endian format), 
    string parameters are converted into byte array with ASCII encoding.
    (CommandIds class contains the tcp command ids)
*/

//MARK: - Command ID-s
class CommandIds
{
public:
    static const uint8 CHECK_IN = 0x30;
	static const uint8 START = 0xA1;
	static const uint8 STOP = 0xA2;
	static const uint8 EXIT = 0xA3;
	static const uint8 RESET_PORTS = 0x01;
	static const uint8 SET_SIMU_INPUT_PORT = 0x10;
	static const uint8 SET_GAME_INPUT_PORT = 0x11;
	static const uint8 SET_GAME_IP_ADDRESS = 0xA4;
	static const uint8 SET_OUTPUT_PORT = 0x12;
	static const uint8 SET_YAW_PID = 0x99;
	static const uint8 SET_PITCH_PID = 0x9A;
	static const uint8 SET_ROLL_PID = 0x9B;
	static const uint8 SET_GAME_MODE = 0x80;
	static const uint8 GET_GAME_PARAMS = 0x81;
	static const uint8 SET_POWER = 0x30;
	static const uint8 SET_TILT_LIMITS = 0x40;
	static const uint8 SET_YAW_LIMIT = 0x70;
	static const uint8 SET_YAW_LIMIT_SPEED = 0x71;
	static const uint8 SET_LED_STRIP_COLOR = 0xB0;
	static const uint8 SET_LED_STRIP_MODE = 0xB1;
	static const uint8 CHECK_IN_ANS = 0x31;
	static const uint8 ERROR = 0xA5;
	static const uint8 SERVER_PID_PARAMS = 0xFF;
};


class Commands
{
private:
    Commands ();

    static FString FormatRotation(float f);

	static FString FormatInt(int i);

    static TArray<uint8> IntToByteArray(int intValue);

    static TArray<uint8> FloatToByteArray(float floatValue);

public:
	static const TArray<uint8> START;

	static const TArray<uint8> STOP;

	static const TArray<uint8> EXIT;

    //MARK: - CALLS FROM GAME TO SIMULATOR
    //UDP
    static const TArray<uint8> DEVICE_DISCOVERY;
      
    //example: "Y[000.00]P[359.99]R[180.00]"; - there is no 360.00, just 000.00
    static TArray<uint8> SET_POSITION(float yaw, float pitch, float roll, int rightBuzzer, int centerBuzzer, int leftBuzzer, int buzzerHz);

    //TCP

    static TArray<uint8> CHECK_IN(int udpListeningPort, FString gameName);

    static TArray<uint8> SET_TILT_LIMITS(int pitchFrontMax, int pitchBackMax, int rollMax);

    static TArray<uint8> SET_YAW_LIMIT(int yawMax);

    //MARK: - Helper functions
    static int ByteArrayToInt(TArray<uint8> intBytes, int startIndex);

	static float ByteArrayToFloat(TArray<uint8> floatBytes, int startIndex);

	static TArray<uint8> FStringToBytes(FString Message);
};

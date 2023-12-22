#pragma once

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1479271-udp-server-in-c
// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/93014-local-udp-broadcast-with-fsocket

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/10427-tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script?18566-TCP-Socket-Listener-Receiving-Binary-Data-into-UE4-From-a-Python-Script=
// https://wiki.unrealengine.com/UDP_Socket_Sender_Receiver_From_One_UE4_Instance_To_Another

#include "Networking.h"

class NetworkTestTCP {
public:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	void Start();

	bool StartTCPReceiver(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort
	);

	FSocket* CreateTCPConnectionListener(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort,
		const int32 ReceiveBufferSize = 2 * 1024 * 1024
	);

	//Timer functions, could be threads
	void TCPConnectionListener(); 	//can thread this eventually
	void TCPSocketListener();		//can thread this eventually


	//Format String IP4 to number array
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);

	//Rama's StringFromBinaryArray
	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
};
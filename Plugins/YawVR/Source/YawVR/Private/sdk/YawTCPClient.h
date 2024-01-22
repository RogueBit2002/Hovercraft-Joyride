#pragma once

#include "YawVR.h"

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1479271-udp-server-in-c
// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/93014-local-udp-broadcast-with-fsocket

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/10427-tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script?18566-TCP-Socket-Listener-Receiving-Binary-Data-into-UE4-From-a-Python-Script=
// https://wiki.unrealengine.com/UDP_Socket_Sender_Receiver_From_One_UE4_Instance_To_Another

// https://answers.unrealengine.com/questions/756684/send-udp-data-via-broadcast.html

#include "Networking.h"

class YawTCPClientDelegate
{
public:
	virtual void DidRecieveTCPMessage(TArray<uint8> data) = 0;
	virtual void DidLostServerConnection() = 0;
};

class YawTCPClient
{
public:
	YawTCPClient() {}

	virtual ~YawTCPClient();

	FSocket* Socket = NULL;

	YawTCPClientDelegate* tcpDelegate = NULL;
	bool connected = false;

	void Initialize(FString ip, int port);

	void CloseConnection();

	void Receive();

	void Send(const TArray<uint8> &data);
};
#pragma once

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1479271-udp-server-in-c
// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/93014-local-udp-broadcast-with-fsocket

// https://forums.unrealengine.com/development-discussion/c-gameplay-programming/10427-tcp-socket-listener-receiving-binary-data-into-ue4-from-a-python-script?18566-TCP-Socket-Listener-Receiving-Binary-Data-into-UE4-From-a-Python-Script=
// https://wiki.unrealengine.com/UDP_Socket_Sender_Receiver_From_One_UE4_Instance_To_Another

// https://answers.unrealengine.com/questions/756684/send-udp-data-via-broadcast.html

#include "Networking.h"

#include "string"

using std::string;

class YawUDPClientDelegate
{
	void DidRecieveUDPMessage(string message, TSharedPtr<FInternetAddr> remoteEndPoint);
};

class YawUDPClient {
public:
	int listeningPort;

	FSocket* Socket;
	FUdpSocketReceiver* udpClient;

	TSharedPtr<FInternetAddr> remoteEndPoint;
	YawUDPClientDelegate udpDelegate;
	//IAsyncResult ar_ = null;

	YawUDPClient(int listeningPort) {
		this->listeningPort = listeningPort;

		int32 BufferSize = 2 * 1024 * 1024;
		Socket = FUdpSocketBuilder(TEXT("UDP Socket"))
			.AsReusable()
			.AsNonBlocking()
			.WithBroadcast()
			.WithSendBufferSize(BufferSize)
			.WithReceiveBufferSize(BufferSize)
			.BoundToAddress(FIPv4Address::Any)
			.BoundToEndpoint(FIPv4Endpoint::Any)
			.BoundToPort(listeningPort)
			.Build();

		udpClient = new FUdpSocketReceiver(Socket, FTimespan(0, 0, 0, 0, 200), TEXT("UDP Socket"));
	}

	void SetRemoteEndPoint(FString ipAddress, int port) {
		remoteEndPoint = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool bIsValid = false;
		remoteEndPoint->SetIp(*ipAddress, bIsValid); // Destination IP
		remoteEndPoint->SetPort(port); // Destination port
	}

	void StartListening()
	{
		StartListeningToMessages();
	}
	void StopListening()
	{
		Socket->Close();
		udpClient->Stop();
	}

	void StartListeningToMessages()
	{
		udpClient->OnDataReceived().BindRaw(this, &YawUDPClient::Receive);
		udpClient->Start();
	}
	void Receive(const FArrayReaderPtr& Data, const FIPv4Endpoint& FromAddress)
	{
		UE_LOG(LogTemp, Log, TEXT("Data Received: %i Bytes"), Data->Num());

		/*
		IPEndPoint ipEndPoint = new IPEndPoint(IPAddress.Any, this->listeningPort);
		byte[] bytes = udpClient.EndReceive(ar, ref ipEndPoint);
		string message = Encoding.ASCII.GetString(bytes);

		if (!message.Contains("YAW_CALLING")) {
			ActionBus.Instance().Add(() = >
			{
				udpDelegate.DidRecieveUDPMessage(message, ipEndPoint);
			});
		}
		StartListeningToMessages();
		*/
	}

	void SendBroadcast(int port, uint8* data, uint32 size)
	{
		TSharedPtr<FInternetAddr> remoteEndPoint = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool bIsValid = false;
		remoteEndPoint->SetIp(TEXT("255.255.255.255"), bIsValid);
		remoteEndPoint->SetPort(port);

		remoteEndPoint->SetBroadcastAddress();
		remoteEndPoint->SetPort(port);

		/*
		FString Message;
		data = (uint8*)TCHAR_TO_UTF8(Message.GetCharArray().GetData());
		*/

		int32 BytesSent;

		Socket->SendTo(data, size, BytesSent, *remoteEndPoint);

		if (size != BytesSent) {
			ESocketErrors ErrorCode = ISocketSubsystem::Get()->GetLastErrorCode();
			UE_LOG(LogTemp, Error, TEXT("(%i) Failed to send %i bytes of data.  Sent %i bytes."), int32(ErrorCode), size, BytesSent);
		}
	}


	void Send(uint8* data, uint32 size)
	{
		/*
		FString Message;
		data = (uint8*)TCHAR_TO_UTF8(Message.GetCharArray().GetData());
		*/

		int32 BytesSent;

		Socket->SendTo(data, size, BytesSent, *remoteEndPoint);

		if (size != BytesSent) {
			ESocketErrors ErrorCode = ISocketSubsystem::Get()->GetLastErrorCode();
			UE_LOG(LogTemp, Error, TEXT("(%i) Failed to send %i bytes of data.  Sent %i bytes."), int32(ErrorCode), size, BytesSent);
		}
	}
};
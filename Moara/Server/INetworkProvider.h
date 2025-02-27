#pragma once

#include <string>
#include <functional>

enum class ENetworkProvider
{
	Sfml,
	Qt
};

using MessageReceivedCallback = std::function<void(int, const std::string&)>;
using ConnectionDisconnectionCallback = std::function<void(int)>;

class INetworkProvider 
{
public:
	virtual ~INetworkProvider() = default;

	static INetworkProvider* Produce(ENetworkProvider provider);

	virtual void SetClientMessageHandler(MessageReceivedCallback handler) = 0;
	virtual void SetDisconnectionHandler(ConnectionDisconnectionCallback handler) = 0;
	virtual void SetConnectionHandler(ConnectionDisconnectionCallback handler) = 0;

	virtual bool Init(int port) = 0;
	virtual void Run() = 0;
	virtual bool SendToClient(int clientId, const std::string& message) = 0;
};
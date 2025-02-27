#pragma once

#include <string>
#include <functional>

enum class ENetworkProvider
{
	Sfml,
	Qt
};

using MessageReceivedCallback = std::function<void(const std::string&)>;

class INetworkProvider
{
public:
	static INetworkProvider* Produce(ENetworkProvider provider);

	virtual bool ConnectToServer(const std::string& host, int port) = 0;
	virtual void DisconnectFromServer() = 0;

	virtual void SetMessageReceivedCb(MessageReceivedCallback callback) = 0;

	virtual bool SendMessage(const std::string& message) = 0;

	virtual ~INetworkProvider() = default;
};
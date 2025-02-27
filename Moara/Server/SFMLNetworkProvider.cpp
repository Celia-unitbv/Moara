#include "SFMLNetworkProvider.h"

#include <iostream>

INetworkProvider* INetworkProvider::Produce(ENetworkProvider provider)
{
	switch (provider)
	{
	case ENetworkProvider::Sfml:
		return new SFMLNetworkProvider();
	case ENetworkProvider::Qt:
	default:
		return nullptr;
	}
}

SFMLNetworkProvider::SFMLNetworkProvider()
	: m_clientId(0)
{
}

bool SFMLNetworkProvider::Init(int port)
{
	if (listener.listen(port) != sf::Socket::Done)
		return false;

	selector.add(listener);
	return true;
}

void SFMLNetworkProvider::Run()
{
	while (true)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				AcceptNewClient();
			}
			else
			{
				ReceiveFromClients();
			}
		}
	}
}

void SFMLNetworkProvider::AcceptNewClient()
{
	auto client = new sf::TcpSocket;

	if (listener.accept(*client) == sf::Socket::Done)
	{
		selector.add(*client);
		clients[m_clientId] = client;

		connectionHandler(m_clientId);

		std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
		m_clientId++;
	}
	else
	{
		delete client;
	}
}

void SFMLNetworkProvider::ReceiveFromClients()
{
	for (auto it = clients.begin(); it != clients.end();)
	{
		sf::TcpSocket& client = *it->second;
		if (selector.isReady(client))
		{
			sf::Packet packet;
			sf::Socket::Status status = client.receive(packet);

			if (status == sf::Socket::Done)
			{
				std::string message;
				packet >> message;
				if (clientMessageHandler)
					clientMessageHandler(it->first, message);
				++it;
			}
			else if (status == sf::Socket::Disconnected)
			{
				if (disconnectionHandler)
					disconnectionHandler(it->first);
				selector.remove(client);
				delete it->second;
				it = clients.erase(it);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

bool SFMLNetworkProvider::SendToClient(int clientId, const std::string& message)
{
	sf::TcpSocket* client = clients[clientId];
	sf::Packet packet;
	packet << message;

	if (client && client->send(packet) != sf::Socket::Done)
	{
		std::cerr << "Error sending packet to client" << std::endl;
		return false;
	}

	return true;
}

void SFMLNetworkProvider::SetClientMessageHandler(MessageReceivedCallback handler)
{
	clientMessageHandler = handler;
}

void SFMLNetworkProvider::SetDisconnectionHandler(ConnectionDisconnectionCallback handler)
{
	disconnectionHandler = handler;
}

void SFMLNetworkProvider::SetConnectionHandler(ConnectionDisconnectionCallback handler)
{
	connectionHandler = handler;
}

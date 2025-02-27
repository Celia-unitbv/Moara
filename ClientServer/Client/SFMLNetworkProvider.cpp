#include "SfmlNetworkProvider.h"

#include "protocolClient.pb.h"

#include <iostream>
#include <mutex>
#include <condition_variable>

INetworkProvider* INetworkProvider::Produce(ENetworkProvider provider)
{
	switch (provider)
	{
	case ENetworkProvider::Sfml:
		return new SfmlNetworkProvider();
	case ENetworkProvider::Qt:
	default:
		return nullptr;
	}
}

SfmlNetworkProvider::SfmlNetworkProvider()
	: m_running(false)
{

}

bool SfmlNetworkProvider::ConnectToServer(const std::string& host, int port)
{
	if (m_running)
	{
		return false;
	}
	sf::Time timeout = sf::seconds(3);
	if (m_socket.connect(host, port,timeout) == sf::Socket::Done)
	{
		m_running = true;
		m_thread = std::thread(&SfmlNetworkProvider::Process, this);

		return true;
	}

	return false;
}

void SfmlNetworkProvider::DisconnectFromServer()
{
	m_running = false;

	m_socket.disconnect();

	if (m_thread.joinable())
		m_thread.join();
}

bool SfmlNetworkProvider::SendMessage(const std::string& message)
{
	sf::Packet packet;

	packet << message;

	if (m_socket.send(packet) == sf::Socket::Done)
		return true;

	std::cerr << "Error sending join request to server" << std::endl;
	return false;
}

void SfmlNetworkProvider::Process()
{
	while (m_running)
	{
		sf::Packet responsePacket;

		sf::Socket::Status status = m_socket.receive(responsePacket);

		if (status == sf::Socket::Done)
		{
			std::string message;
			responsePacket >> message;

			m_messageReceivedCb(message);
		}

		if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
		{
			m_running = false;

			Command com;
			com.set_command(Ecommand::ServerDisconnect);

			std::string serialized_command;
			com.SerializeToString(&serialized_command);

			m_messageReceivedCb(serialized_command);
			break;
		}
	}
}

void SfmlNetworkProvider::SetMessageReceivedCb(MessageReceivedCallback callback)
{
	m_messageReceivedCb = callback;
}

#pragma once
#include "IClientListener.h"

#include <memory>
#include <functional>

class IClient;

using IClientPtr = std::shared_ptr<IClient>;
using MessageReceivedCallback = std::function<void(const std::string&)>;

class IClient
{
public:
	virtual ~IClient() = default;

	static IClientPtr Produce();

	//Piece Action
	virtual void PlacePiece(const Pos& placePos) = 0;
	virtual void MovePiece(const Pos& initialPoz, const Pos& finalPoz) = 0;
	virtual void CapturePiece(const Pos& placePos) = 0;
	virtual void ShowHints(const Pos& position) = 0;

	//Buttons
	virtual void Undo() = 0;
	virtual void Reset() = 0;

	//Start-End methods
	virtual void StartGame() = 0;
	virtual void EndGame() = 0;

	//Getters
	virtual void GetGameState() = 0;
	virtual void GetTime() = 0;
	virtual void GetClientPlayer() = 0;

	//Setters
	virtual void SetConnectionInfo(const std::string& host, int port) = 0;
	virtual void SetGameName(const std::string& gameName) = 0;
	virtual void SetStrategy(int strategy) = 0;

	//Lobby functions
	virtual bool Start(bool vsComputer, const std::string& gameName) = 0;

	//Listeners
	virtual void AddListener(IClientListener* listener) = 0;
	virtual void RemoveListener(IClientListener* listener) = 0;
};

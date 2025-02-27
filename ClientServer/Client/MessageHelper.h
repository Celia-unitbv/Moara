#pragma once
#include "protocolClient.pb.h"

#include <string>
#include <vector>

using Pos = std::pair<int, int>;

class MessageHelperClient
{
public:
	MessageHelperClient();

	const std::string GetMessageForWhereToMove(const Pos nod)const;
	const std::string GetMessageForPlacePiece(const Pos position)const;
	const std::string GetMessageForServer(const std::string& gameName, const std::string& message)const;
	const std::string GetMessageForCapturePiece(const Pos position)const;
	const std::string GetMessageForMovePiece(const Pos initialPosition, const Pos finalPosition)const;

	const std::string GetMessageForSave()const;
	const std::string GetMessageForReset()const;
	const std::string GetMessageForStart()const;
	const std::string GetMessageForTime()const;
	const std::string GetMessageForProxyPlayer()const;
	const std::string GetMessageForLoad()const;
	const std::string GetMessageForUndo()const;


	const std::string GetMessageForStrategy(int strategy) const;

	const std::string GetMessageForGameState()const;

private:
	Command* command;
};




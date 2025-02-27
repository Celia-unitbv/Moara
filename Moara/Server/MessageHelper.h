#pragma once

#include "IGame.h"
#include "Using.h"
#include "protocolClient.pb.h"

#include <string>

class MessageHelper
{
public:
	MessageHelper();

	void SetGame(IGamePtr game);

	const std::string GetMessageForPlacePiece(Pos pozition, int color) const;
	const std::string GetMessageForMovePiece(Pos initialPos, Pos finalPos, int currentPlayer) const;
	const std::string GetMessageForCapturePiece(Pos pozition) const;

	const std::string GetMessageForLoad() const;
	const std::string GetMessageForError(int result) const;
	const std::string GetMessageForWin() const;
	const std::string GetMessageForReset() const;

	const std::string GetMessageForWhereToCapture() const;
	const std::string GetMessageForWhereToMove(const Pos position) const;

	const std::string GetMessageForGameState() const;
	const std::string GetMessageForClientPlayer(EPlayer player) const;
	const std::string GetMessageForAllTime() const;

	const std::string GetMessageForundo(const Positions& positions) const;

	static std::vector<int> SplitStringToInts(const std::string& str);

private:
	IGamePtr m_game;
	Command* command;
};


#include "MediumStartegy.h"
#include "Piece.h"
#include <random>
#include <map>

MediumStrategy::MediumStrategy(const Board& board)
	: m_board(board)
{
	//empty
}

std::pair<Pos, Pos> MediumStrategy::MovePiece(EPlayer player)
{
	Positions botPozitions;
	Positions whereToMove;

	std::map<Pos, Positions> botPositionsMap;

	std::vector<std::pair<Pos, Pos>>botLine;
	std::vector<std::pair<Pos, Pos>>lineStop;
	std::vector<std::pair<Pos, Pos>>randomPoz;

	Board tempBoard = m_board;

	for (int i = 0; i < m_board.GetBoard().size(); i++)
	{
		for (int j = 0; j < m_board.GetBoard()[i].size(); j++)
		{
			if (m_board.GetBoard()[i][j] && (int)m_board.GetBoard()[i][j]->GetColor() == (int)player)
			{
				whereToMove = m_board.PossibleMove((int)player, { i, j });
				botPositionsMap[{i, j}] = whereToMove;
				whereToMove.clear();
			}
		}
	}

	for (const auto& MapElement : botPositionsMap)
	{
		Pos key = MapElement.first;
		Positions moves = MapElement.second;
		for (const auto& move : moves)
		{
			Board tempBoard = m_board;
			IPiecePtr p = std::make_shared<Piece>((EColor)player, key.first, key.second);
			tempBoard.MovePiece(move, p,(int)player);

			if (tempBoard.CheckLine(move, (int)player)) {
				botLine.push_back({ key, {move.first, move.second} });
			}

			if (tempBoard.CheckLine(move, 1 - (int)player)) {
				lineStop.push_back({ key, {move.first, move.second} });
			}

			randomPoz.push_back({ key, {move.first, move.second} });
			tempBoard.UndoMove(move, p, (int)player);
		}

	}
	if (botLine.size() != 0)
	{
		return botLine[m_rand.getRandomNumber(botLine.size() - 1)];
	}
	if (lineStop.size() != 0)
	{
		return lineStop[m_rand.getRandomNumber(lineStop.size() - 1)];
	}

	return randomPoz[m_rand.getRandomNumber(randomPoz.size() - 1)];
}

Pos MediumStrategy::CapturePiece(EPlayer player)
{
	Positions positions = m_board.PossibleCapture((int)player);
	return positions[m_rand.getRandomNumber(positions.size() - 1)];
}

Pos MediumStrategy::PlacePiece(EPlayer player)
{
	Positions botPozitions;
	Positions playerPozitions;
	Positions randomPoz;

	for (int i = 0; i < m_board.GetBoard().size(); i++)
	{
		for (int j = 0; j < m_board.GetBoard()[i].size(); j++)
		{
			if (!m_board.GetBoard()[i][j])
			{
				if (m_board.CheckLine({ i, j }, (int)player))
					botPozitions.push_back({ i,j });

				if (m_board.CheckLine({ i, j }, 1 - (int)player))
					playerPozitions.push_back({ i,j });

				randomPoz.emplace_back(std::make_pair(i, j));

			}
		}
	}

	if (botPozitions.size() != 0)
	{
		return botPozitions[m_rand.getRandomNumber(botPozitions.size() - 1)];
	}
	if (playerPozitions.size() != 0)
	{
		return playerPozitions[m_rand.getRandomNumber(playerPozitions.size() - 1)];
	}

	return randomPoz[m_rand.getRandomNumber(randomPoz.size() - 1)];
}

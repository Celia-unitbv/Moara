#include "EasyStrategy.h"
#include <random>

EasyStrategy::EasyStrategy(const Board& board) :
	m_board(board)
{
	//empty
}

std::pair<Pos, Pos> EasyStrategy::MovePiece(EPlayer player)
{
	int randomIndex;
	std::vector<Pos>WhereToMove;
	std::vector<Pos>positions;

	for (int i = 0; i < m_board.GetBoard().size(); i++)
	{
		for (int j = 0; j < m_board.GetBoard()[i].size(); j++)
		{
			if (m_board.GetBoard()[i][j] && (int)m_board.GetBoard()[i][j]->GetColor() == (int)player)
			{
				positions.push_back({ i, j });
			}
		}
	}

	do
	{
		randomIndex = m_rand.getRandomNumber(positions.size() - 1);

		WhereToMove = m_board.PossibleMove(int(player), positions[randomIndex]);

	} while (WhereToMove.size() == 0);

	return std::make_pair(positions[randomIndex], WhereToMove[m_rand.getRandomNumber(WhereToMove.size() - 1)]);
}

Pos EasyStrategy::CapturePiece(EPlayer player)
{
	std::vector<Pos>positions = m_board.PossibleCapture((int)player);
	return positions[m_rand.getRandomNumber(positions.size() - 1)];
}

Pos EasyStrategy::PlacePiece(EPlayer player)
{
	std::vector<Pos>positions;

	for (int i = 0; i < m_board.GetBoard().size(); i++)
	{
		for (int j = 0; j < m_board.GetBoard()[i].size(); j++)
		{
			if (!m_board.GetBoard()[i][j])
			{
				positions.push_back({ i, j });
			}
		}
	}
	return positions[m_rand.getRandomNumber(positions.size() - 1)];
}
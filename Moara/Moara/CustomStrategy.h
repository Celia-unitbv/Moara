#pragma once
#include "IStrategy.h"
#include "Board.h"

class CustomStrategy : public IStrategy
{
private:
	const BoardMatrix& m_board;

public:
	CustomStrategy(const BoardMatrix& board) : m_board(board)
	{
		//empty
	}

	std::pair<Pos, Pos> MovePiece(EPlayer player) override
	{
		std::vector<Pos>positions;
		Pos firstFree = { -1,-1 };

		for (int i = 0; i < m_board.size(); i++)
		{
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (m_board[i][j] && (int)m_board[i][j]->GetColor() == (int)player)
				{
					positions.push_back({ i, j });
				}
				else if (firstFree .first==-1 && firstFree.second==-1)
				{
					firstFree.first = i;
					firstFree.second = j;
				}
			}
		}

		return std::make_pair(positions[0], firstFree);

	}

	Pos CapturePiece(EPlayer player) override
	{
		std::vector<Pos>positions;
		for (int i = 0; i < m_board.size(); i++)
		{
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (m_board[i][j] && (int)m_board[i][j]->GetColor() != (int)player)
				{
					positions.push_back({ i, j });
				}
			}
		}
		return positions[0];
	}

	Pos PlacePiece(EPlayer player) override
	{
		for (int i = 0; i < m_board.size(); i++)
		{
			for (int j = 0; j < m_board[i].size(); j++)
			{
				if (!m_board[i][j])
				{
					return{ i, j };
				}
			}
		}

		return {-1, -1};
	}

};
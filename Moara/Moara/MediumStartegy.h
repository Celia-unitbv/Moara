#pragma once

#include "IStrategy.h"
#include "Board.h"
#include "Random.h"

class MediumStrategy : public IStrategy
{
public:
	MediumStrategy(const Board& board);

	std::pair< Pos, Pos> MovePiece(EPlayer player) override;

	Pos CapturePiece(EPlayer player) override;

	Pos PlacePiece(EPlayer player) override;

private:
	const Board& m_board;
	static RandomGenerator m_rand;
};



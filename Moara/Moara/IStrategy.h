#pragma once

#include "EPlayer.h"
#include "Using.h"

#include <memory>

/// <summary>
/// Interface for defining game strategies.
/// </summary>
class IStrategy
{
public:
	/// <summary>
	/// Determines the move action for a given player.
	/// </summary>
	/// <param name="player">The player making the move.</param>
	/// <returns>A pair of positions representing the initial and final positions of the piece being moved.</returns>
	virtual std::pair<Pos, Pos> MovePiece(EPlayer player) = 0;

	/// <summary>
	/// Determines the capture action for a given player.
	/// </summary>
	/// <param name="player">The player performing the capture.</param>
	/// <returns>The position of the piece being captured.</returns>
	virtual Pos CapturePiece(EPlayer player) = 0;

	/// <summary>
	/// Determines the placement action for a given player.
	/// </summary>
	/// <param name="player">The player placing the piece.</param>
	/// <returns>The position where the piece should be placed.</returns>
	virtual Pos PlacePiece(EPlayer player) = 0;

	/// <summary>
	/// Virtual destructor for the IStrategy interface.
	/// </summary>
	virtual ~IStrategy() = default;
};

using IStrategyPtr = std::shared_ptr<IStrategy>;
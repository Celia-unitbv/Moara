#pragma once

#include "EColor.h"
#include "Using.h"

#include <utility>
#include <memory>

/// <summary>
/// Interface representing a game piece.
/// </summary>
class IPiece
{
public:
	/// <summary>
	/// Gets the color of the piece.
	/// </summary>
	/// <returns>The color of the piece as EColor enum.</returns>
	virtual EColor GetColor() const = 0;

	/// <summary>
	/// Checks if the piece can be placed at the given position.
	/// </summary>
	/// <param name="position">The position to check for placement.</param>
	/// <returns>True if the piece can be placed at the given position, false otherwise.</returns>
	virtual bool CheckPlace(Pos position) const = 0;

	/// <summary>
	/// Gets the current position of the piece.
	/// </summary>
	/// <returns>A pair representing the current position of the piece.</returns>
	virtual Pos GetPozition() const = 0;

	/// <summary>
	/// Sets the position of the piece.
	/// </summary>
	/// <param name="position">The new position to set for the piece.</param>
	virtual void SetPozition(Pos position) = 0;

	/// <summary>
	/// Virtual destructor for the IPiece interface.
	/// </summary>
	virtual ~IPiece() = default;
};

using IPiecePtr = std::shared_ptr<IPiece>;




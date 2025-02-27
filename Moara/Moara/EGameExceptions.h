#pragma once

/// <summary>
/// Enum representing the possible exceptions for actions ( place, capture, move).
/// </summary>
enum class EOperationResult 
{
	/// <summary>
	/// Represents that the action was made successfully.
	/// </summary>
	NoError,

	/// <summary>
	/// Represents that a piece is in a line with two other pieces of the same color.
	/// </summary>
	PieceIsPartOfALine,

	/// <summary>
	/// Represents a place that is not valid for an action.
	/// </summary>
	IsNotAValidPlace,

	/// <summary>
	/// Represents a position outside the board.
	/// </summary>
	IndexOutOfBounds,

	/// <summary>
	/// Represents a selected piece which doesn't belong to the current player.
	/// </summary>
	IsNotAValidPiece,

	InvalidCommand
};
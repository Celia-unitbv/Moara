#pragma once

/// <summary>
/// Enum class representing the states of the game.
/// </summary>
enum class EState
{
	/// <summary>
	/// Represents the state when Player 1 has won.
	/// </summary>
	Player1Won,

	/// <summary>
	/// Represents the state when Player 2 has won.
	/// </summary>
	Player2Won,

	/// <summary>
	/// Represents the state when a player can place a piece.
	/// </summary>
	Place,

	/// <summary>
	/// Represents the state when a player can move a piece.
	/// </summary>
	Move,

	/// <summary>
	/// Represents the state when a player can capture a piece.
	/// </summary>
	Capture
};



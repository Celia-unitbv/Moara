#pragma once


/// <summary>
/// Enum class representing the difficulty levels for a game.
/// </summary>
enum class EGameType
{
	/// <summary>
	/// Represents the easy difficulty level. The computer is choosing randomly a position for an action.
	/// </summary>
	Easy,

	/// <summary>
	/// Represents the medium difficulty level. The computer is calculating the best position to either make a line for himslef, or block a line from the opponent.
	/// </summary>
	Medium
};
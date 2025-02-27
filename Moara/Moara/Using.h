#include <vector>
#include <chrono>

/// <summary>
/// Type alias for a collection of positions on the board.
/// </summary>
/// <remarks>
/// This is a vector of <see cref="Pos"/> where each <see cref="Pos"/> represents a position on the board.
/// </remarks>
using Positions = std::vector<std::pair<int, int>>;

/// <summary>
/// Type alias for representing a position on the board as a pair of integers.
/// </summary>
/// <remarks>
/// The first integer represents the x-coordinate (or row), and the second integer represents the y-coordinate (or column).
/// </remarks>
using Pos = std::pair<int, int>;

/// <summary>
/// Type alias for representing a duration in milliseconds.
/// </summary>
/// <remarks>
/// This alias is used to simplify the usage of std::chrono::milliseconds
/// and improve code readability when dealing with time durations.
/// </remarks>
using Miliseconds = std::chrono::milliseconds;
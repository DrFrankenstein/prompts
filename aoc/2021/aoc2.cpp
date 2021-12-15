#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

using std::cin, std::cout, std::for_each, std::istream, std::istream_iterator, std::runtime_error, std::string, std::string_view;

enum class Direction { FORWARD, DOWN, UP };
struct Command { Direction direction; int distance; };

inline auto parseDirection(string_view dirstr)
{
	if (dirstr == "forward")
		return Direction::FORWARD;
	if (dirstr == "up")
		return Direction::UP;
	if (dirstr == "down")
		return Direction::DOWN;

	throw runtime_error { "invalid direction" };
}

inline auto operator >>(istream& input, Direction& direction) -> istream&
{
	string dirstr;
	input >> dirstr;
	if (!input)
		return input; // that was the end of it

	direction = parseDirection(dirstr);
	return input;
}

inline auto operator >>(istream& input, Command& command) -> istream&
{
	return input >> command.direction >> command.distance;
}

class Submarine
{
public:
	auto exec(Command command)
	{
		switch (command.direction)
		{
		case Direction::FORWARD:
			xpos += command.distance;
			break;
		case Direction::UP:
			depth -= command.distance;
			break;
		case Direction::DOWN:
			depth += command.distance;
			break;
		}
	}

	[[nodiscard]] auto positionDigest() const
	{
		return xpos * depth;
	}

private:
	int xpos = 0;
	int depth = 0;
};

auto main() -> int
{
	Submarine sub;
	istream_iterator<Command> input { cin }, end;
	for_each(input, end, [&](auto command) {
		sub.exec(command);
	});

	cout << sub.positionDigest() << '\n';
}

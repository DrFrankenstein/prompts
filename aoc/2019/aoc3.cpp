#include <cmath>
#include <exception>
#include <iostream>
#include <iterator>
#include <list>
#include <type_traits>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/min_element.hpp>
#include <boost/range/adaptor/transformed.hpp>

using std::abs, std::back_inserter, std::cin, std::cout, std::end, std::endl,
	std::istream, std::list, std::make_signed, std::noskipws, std::runtime_error,
	std::size_t, std::unordered_map;
using boost::forward_traversal_tag, boost::iterator_core_access, boost::iterator_facade;
using boost::iterator_range, boost::min_element, boost::adaptors::transformed;
using boost::hash, boost::hash_combine;

using Distance = unsigned int;
using Wire = unsigned char;

struct Position
{
	Distance x, y;

	bool operator ==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}

	friend size_t hash_value(const Position& position)
	{
		size_t hash;
		hash_combine(hash, position.x);
		hash_combine(hash, position.y);

		return hash;
	}
};

enum class Direction : char { UP = 'U', DOWN = 'D', LEFT = 'L', RIGHT = 'R' };
struct Command
{
	Direction direction;
	Distance distance;
};

class CommandIterator 
	: public iterator_facade<CommandIterator, Command const, forward_traversal_tag>
{
public:
	CommandIterator() :_input(nullptr) { }
	CommandIterator(istream& input) :_input(&input)
	{
		readCommand();
	}

private:
	friend class iterator_core_access;

	void increment()
	{
		char c;
		*_input >> noskipws >> c;

		if (_input->eof() || c == '\n')
			_input = nullptr;
		else if (c == ',')
			readCommand();
		else
			throw runtime_error { "expected ',', newline, or end of file" };
	}

	void readCommand()
	{
		if (!_input)
			return;

		char c;
		*_input >> c;

		if (!*_input)
		{
			_input = nullptr;
			return;
		}

		_command.direction = static_cast<Direction>(c);
		
		*_input >> _command.distance;
	}

	bool equal(CommandIterator const& other) const
	{
		return _input == other._input;
	}

	const Command& dereference() const
	{
		return _command;
	}

	istream* _input;
	Command _command {Direction::UP, 0};
};

template <typename CrossingsIterator>
class CrossingFinder
{
public:
	CrossingFinder(CrossingsIterator& inserter)
		: _crossings(inserter)
	{ }

	template<typename Range>
	void addWire(Range commandRange)
	{
		++_currentWire;

		Position position { 0, 0 };
		for (const auto& command : commandRange)
			walk(position, command);
	}

private:
	void walk(Position& position, const Command& command)
	{
		for (auto count = command.distance; count != 0; --count)
			step(position, command.direction);
	}

	void step(Position& position, Direction direction)
	{
		switch (direction)
		{
		case Direction::UP:
			++position.y;
			break;
		case Direction::DOWN:
			--position.y;
			break;
		case Direction::LEFT:
			--position.x;
			break;
		case Direction::RIGHT:
			++position.x;
			break;

		default:
			throw runtime_error { "bad direction" };
		}

		if (isCrossing(position))
			*_crossings++ = position;

		_traces.insert({ position, _currentWire });
	}

	bool isCrossing(const Position& position)
	{
		auto crossing = _traces.find(position);
		return crossing != end(_traces) && crossing->second != _currentWire;
	}

	using Traces = unordered_map<Position, Wire, hash<Position>>;

	CrossingsIterator& _crossings;
	Traces _traces {};
	Wire _currentWire = 0;
};

Distance distanceTo(const Position& origin, const Position& destination)
{
	using DistanceDiff = make_signed<Distance>::type;
	DistanceDiff dx = origin.x - destination.x,
	             dy = origin.y - destination.y;

	return abs(dx) + abs(dy);
}

int main()
{
	list<Position> crossings;

	auto inserter = back_inserter(crossings);
	CrossingFinder finder { inserter };
	
	while (!cin.eof())
	{
		CommandIterator commands{ cin };
		finder.addWire(iterator_range<CommandIterator>(commands, CommandIterator{}));
	}

	auto distances = crossings | transformed([](auto crossing) { return distanceTo({ 0, 0 }, crossing); });
	auto minDistance = *min_element(distances);

	cout << "Closest crossing is at " << minDistance << endl;
}

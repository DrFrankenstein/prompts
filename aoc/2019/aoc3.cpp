#include <cmath>
#include <exception>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <boost/functional/hash.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/min_element.hpp>

using std::abs, std::cin, std::cout, std::end, std::endl,
	std::istream, std::noskipws, std::runtime_error,
	std::size_t, std::unordered_map, std::unordered_set;
using boost::forward_traversal_tag, boost::iterator_core_access, boost::iterator_facade;
using boost::make_iterator_range, boost::min_element, boost::adaptors::transformed;
using boost::hash, boost::hash_combine;

using Distance = int;
struct Position
{
	Distance x, y;

	bool operator ==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}

	friend size_t hash_value(const Position& position)
	{
		size_t hash = 0;
		hash_combine(hash, position.x);
		hash_combine(hash, position.y);

		return hash;
	}
};

using Wire = unsigned char;
struct Trace
{
	Wire wire;
	Distance distance;
};

struct Crossing
{
	Position position;
	Distance combinedDistances;
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
	CommandIterator() = default;
	explicit CommandIterator(istream& input) : _input(&input)
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
		if (_input == nullptr)
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

	[[nodiscard]] bool equal(CommandIterator const& other) const
	{
		return _input == other._input;
	}

	[[nodiscard]] const Command& dereference() const
	{
		return _command;
	}

	istream* _input { nullptr };
	Command _command {Direction::UP, 0};
};

class CrossingFinder
{
public:
	CrossingFinder() = default;

	template<typename Range>
	void addWire(Range commandRange)
	{
		++_currentWire;
		_currentDistance = 0;

		Position position { 0, 0 };
		for (const auto& command : commandRange)
			walk(position, command);
	}

	[[nodiscard]] auto crossings() const
	{
		return _crossings | transformed([this](auto position) {
			auto trace = _traces.at(position);
			return Crossing { position, trace.distance };
		});
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
		{
			_crossings.insert(position);
			auto& trace = _traces.at(position);
			trace.distance += ++_currentDistance;
		}
		else
			_traces.insert({ position, { _currentWire, ++_currentDistance } });
	}

	bool isCrossing(const Position& position)
	{
		auto crossing = _traces.find(position);
		
		if (crossing == end(_traces))
			return false;

		auto& trace = crossing->second;
		return trace.wire != _currentWire;
	}

	using Crossings = unordered_set<Position, hash<Position>>;
	using Traces = unordered_map<Position, Trace, hash<Position>>;

	Crossings _crossings;
	Traces _traces {};
	Wire _currentWire = 0;
	Distance _currentDistance = 0;
};

Distance distanceTo(const Position& origin, const Position& destination)
{
	Distance dx = origin.x - destination.x,
	         dy = origin.y - destination.y;

	return abs(dx) + abs(dy);
}

int main()
{
	CrossingFinder finder{};

	while (!cin.eof())
	{
		CommandIterator commands{ cin };
		finder.addWire(make_iterator_range(commands, {}));
	}

	auto crossings = finder.crossings();
	auto distancesFromOrigin = crossings | transformed([](auto crossing) { return distanceTo({ 0, 0 }, crossing.position); });
	auto minDistance = *min_element(distancesFromOrigin);

	cout << "Closest crossing is at " << minDistance << endl;

	auto distancesAtCrossing = crossings | transformed([](auto crossing) { return crossing.combinedDistances; });
	auto minCombinedDistance = *min_element(distancesAtCrossing);
	
	cout << "Fastest crossing is at " << minCombinedDistance << endl;
}

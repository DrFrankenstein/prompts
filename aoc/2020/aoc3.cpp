#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::accumulate, std::array, std::begin, std::cin, std::cout, std::end, std::getline,
      std::istream, std::multiplies, std::pair, std::size_t, std::string, std::vector;

#if __cpp_lib_ranges >= 201911L
using std::views::transform;
#else
using nano::views::transform;
#endif

class Map
{
public:
	void parse(istream& input)
	{
		string line;
		while (getline(input, line))
		{
			addRow(line);
		}
	}

	size_t width() const
	{
		if (_data.empty())
			return 0;

		// assumes all the rows are the same size
		return _data[0].size();
	}

	size_t height() const
	{
		return _data.size();
	}

	bool operator ()(size_t x, size_t y) const
	{
		const string& row = _data[y];
		const char cell = row[normalizeX(x)];

		return cell == '#';
	}

private:
	void addRow(const string& row)
	{
		_data.push_back(row);
	}

	size_t normalizeX(size_t x) const
	{
		return x % width();
	}

	// exercise for the reader: use a uint32_t, bitset, or vector<bool>
	// to store rows
	vector<string> _data{};
};

unsigned slideThrough(const Map& map, size_t dx, size_t dy)
{
	unsigned treeCount = 0;
	for (size_t y = 0, x = 0; y < map.height(); y += dy, x += dx)
		if (map(x, y))
			++treeCount;

	return treeCount;
}

int main()
{
	Map map;
	map.parse(cin);

	const auto treeCount = slideThrough(map, 3, 1);
	cout << "(3, 1): We hit " << treeCount << " trees.\n";

	constexpr auto slopes = array {
		pair { 1u, 1u },
		pair { 3u, 1u },
		pair { 5u, 1u },
		pair { 7u, 1u },
		pair { 1u, 2u }
	};

	const auto hitCounts = slopes | transform([&](auto slope) {
		auto [dx, dy] = slope;
		return slideThrough(map, dx, dy);
	});
	const auto product = accumulate(begin(hitCounts), end(hitCounts), 1, multiplies{});	// no ranges::accumulate? :(

	cout << "Tree hit product is " << product << ".\n";
}

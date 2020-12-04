#include <iostream>
#include <string>
#include <vector>

using std::cin, std::cout, std::getline, std::istream, std::size_t, std::string, std::vector;

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
	void addRow(string row)
	{
		_data.push_back(row);
	}

	size_t normalizeX(size_t x) const
	{
		return x % width();
	}

	vector<string> _data{};
};

unsigned slideThrough(const Map& map, int dx, int dy)
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

	auto treeCount = slideThrough(map, 3, 1);

	cout << "We hit " << treeCount << " trees.\n";
}

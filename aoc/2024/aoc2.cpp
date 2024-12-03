#include <algorithm>
#include <cmath>
#include <iostream>
#include <ranges>
#include <utility>
#include <vector>

using namespace std;	// sue me

class Report
{
public:
	Report() {}
	Report(const Report& src)
		: _levels(src._levels)
	{}
	Report(Report&& src)
		: _levels(move(src._levels))
	{}
	~Report() = default;

	Report& operator =(const Report& rhs) { _levels = rhs._levels; return *this; }
	Report& operator =(const Report&& rhs) { _levels = move(rhs._levels); return *this; }

	Report(vector<int>&& levels)
		: _levels(levels)
	{}

	auto isSafe() const
	{
		return ranges::all_of(_levels | views::adjacent<2>, [diff = 0](auto pair) mutable {
			auto const [prev, curr] = pair;
			auto const prevDiff = exchange(diff, curr - prev);
			return sameDirection(prevDiff, diff) && safeDifference(diff);
		});
	}

private:
	static auto sameDirection(int prevDiff, int currDiff) -> bool
	{
		return prevDiff == 0 || signbit(prevDiff) == signbit(currDiff);
	}

	static auto safeDifference(int diff) -> bool
	{
		return diff != 0 && abs(diff) <= 3;
	}

	vector<int> _levels;
};

auto operator >>(istream& input, Report& report) -> istream&
{
	vector<int> levels;

	input >> noskipws;
	char sep = ' ';
	while (input && sep != '\n')
	{
		int level = 0;
		input >> level >> sep;

		if (input) [[likely]]
			levels.push_back(level);
	}

	report = Report(move(levels));
	return input >> skipws;
}

int main()
{
	auto reports = views::istream<Report>(cin);
	cout << ranges::count_if(reports, [](const auto& report) { return report.isSafe(); })
		<< '\n';
}

#include <algorithm>
#include <bitset>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::bitset, std::cin, std::cout, std::istream_iterator, std::runtime_error, std::string;

#if __cpp_lib_ranges >= 201911L
using std::ranges::max, std::ranges::subrange, std::views::transform;
#else
using nano::ranges::max, nano::ranges::subrange, nano::views::transform;
#endif

struct BoardingPass
{
	// we have to parse them separately, so might as well store them separately
	unsigned long _row, _seat;

	BoardingPass(const string& data)
	{
		if (data.size() != 10)
			throw runtime_error("parse error: boarding pass data must be 10 characters long");

		_row = bitset<7>(data, 0, 7, 'F', 'B').to_ulong();
		_seat = bitset<3>(data, 7, 3, 'L', 'R').to_ulong();
	}

	auto id() const {
		return _row << 3 | _seat;
	}
};

auto main() -> int
{
	const auto data = subrange { istream_iterator<string>{cin}, istream_iterator<string>{} };
	const auto passes = data | transform([](const auto& data) { return BoardingPass { data }; });
	const auto ids = passes | transform([](const auto& pass) { return pass.id(); });

	cout << "Highest boarding pass ID is " << max(ids) << ".\n";
}

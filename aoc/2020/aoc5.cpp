#include <algorithm>
#include <bitset>
#include <exception>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::bitset, std::cin, std::cout, std::istream_iterator, std::runtime_error, std::set, std::string;

#if __cpp_lib_ranges >= 201911L
using std::ranges::cbegin, std::ranges::cend, std::ranges::copy, std::views::iota, std::ranges::max, std::ranges::mismatch, std::ranges::subrange, std::views::transform;
#else
using nano::ranges::cbegin, nano::ranges::cend, nano::ranges::copy, nano::views::iota, nano::ranges::max, nano::ranges::mismatch, nano::ranges::subrange, nano::views::transform;
#endif

struct BoardingPass
{
	unsigned long id;

	BoardingPass(const string& data)
	{
		if (data.size() != 10)
			throw runtime_error("parse error: boarding pass data must be 10 characters long");

		const auto row = bitset<7>(data, 0, 7, 'F', 'B').to_ulong();
		const auto seat = bitset<3>(data, 7, 3, 'L', 'R').to_ulong();

		this->id = row << 3 | seat;
	}
};

template <typename Range>
inline auto findGap(const Range& range)
{
	const auto start = *cbegin(range);
	return *mismatch(range, iota(start)).in2;
}

auto main() -> int
{
	const auto data = subrange { istream_iterator<string>{cin}, istream_iterator<string>{} };
	const auto passes = data | transform([](const auto& data) { return BoardingPass { data }; });
	const auto ids = passes | transform([](const auto& pass) { return pass.id; });
	const auto idset = set(cbegin(ids), cend(ids));

	cout << "Highest boarding pass ID is " << max(idset) << ".\n";
	cout << "Your seat ID is " << findGap(idset) << ".\n";
}

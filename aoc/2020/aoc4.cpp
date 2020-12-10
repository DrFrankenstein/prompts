#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::cout, std::runtime_error, std::string, std::pair, std::vector;

#if __cpp_lib_ranges >= 201911L
using std::ranges::cbegin, std::ranges::cend, std::views::common, std::views::split;
#else
using nano::ranges::cbegin, nano::ranges::cend, nano::views::common, nano::views::split;
#endif

namespace Parser
{
	template <typename Range>
	pair<string, string> parsePair(const Range& input)
	{
		const auto splitter = split(input, ':');
		auto it = splitter.begin();
		const auto end = splitter.end();

		if (it == end)
			throw runtime_error("parse error: empty key-value pair");

		const auto ikey = *it++ | common;
		const string key{ cbegin(ikey), cend(ikey) };

		if (it == end)
			throw runtime_error("missing value (no ':')");

		const auto ivalue = *it++ | common;
		const string value{ cbegin(ivalue), cend(ivalue) };

		if (it != end)
			throw runtime_error("stray data in pair (extra ':')");

		return { key, value };
	}
}

int main()
{
	const string foo{ "test:spam" };
	const auto [bar, baz] = Parser::parsePair(foo);

	cout << bar << ", " << baz << '\n';
}

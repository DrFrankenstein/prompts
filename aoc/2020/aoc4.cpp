#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::array, std::cin, std::cout, std::getline, std::inserter, std::istream,
	std::runtime_error, std::string, std::pair, std::map, std::vector;

#if __cpp_lib_ranges >= 201911L
using std::ranges::cbegin, std::ranges::cend, std::views::common, std::ranges::copy,
	std::ranges::count_if, std::ranges::includes, std::views::split, std::views::transform;
#else
using nano::ranges::cbegin, nano::ranges::cend, nano::views::common, nano::ranges::copy,
	nano::ranges::count_if, nano::ranges::includes, nano::views::split, nano::views::transform;
#endif

using Passport = map<string, string>;

namespace Parser
{
	template <typename Range>
	auto parsePair(const Range& input)
	{
		const auto parts = input | split(':');
		auto it = parts.begin();
		const auto end = parts.end();

		if (it == end)
			throw runtime_error("parse error: empty key-value pair");

		const auto ikey = *it++ | common;
		const string key { cbegin(ikey), cend(ikey) };

		if (it == end)
			throw runtime_error("missing value (no ':')");

		const auto ivalue = *it++ | common;
		const string value { cbegin(ivalue), cend(ivalue) };

		if (it != end)
			throw runtime_error("stray data in pair (extra ':')");

		return pair { key, value };
	}

	template <typename Range, typename OutputIterator>
	auto parseLine(const Range& input, OutputIterator it)
	{
		const auto items = input
			| split(' ')
			| transform([](const auto& item) { return parsePair(item); });

		copy(items, it);
	}

	auto parsePassport(istream& input)
	{
		map<string, string> passport;

		string line;
		while (getline(input, line))
		{
			if (line.empty())
				return passport;

			parseLine(line, inserter(passport, passport.end()));
		}

		return passport;
	}

	auto parsePassports(istream& input)
	{
		vector<Passport> passports;

		while (input)
			passports.push_back(parsePassport(input));

		return passports;
	}
}

inline auto validatePassport(const Passport& passport)
{
	constexpr auto expectedKeys = array{
		// must be sorted
		"byr", /*"cid",*/ "ecl", "eyr", "hcl", "hgt", "iyr", "pid"
	};

	const auto keys = passport
		| transform([](const auto& item) -> const string& { return item.first; });

	return includes(keys, expectedKeys);
}

int main()
{
	const auto passports = Parser::parsePassports(cin);
	const auto validCount = count_if(passports, validatePassport);

	cout << "There are " << validCount << " valid passports.\n";
}

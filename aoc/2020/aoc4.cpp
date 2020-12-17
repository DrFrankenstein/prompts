#include <array>
#include <cctype>
#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

// don't import std::isdigit and std::isxdigit here. even though <locale> isn't included, GNU's libstdc++ makes
// those versions available and it results in unresolved overloads down the line when we pass them as arguments.
// there's probably a more elegant fix, but heck it. try a PR and see if I like your solution.
using std::array, std::cin, std::cout, std::getline, std::inserter, std::istream, std::pair, std::runtime_error,
	std::stoul, std::size_t, std::string, std::string_view, std::unordered_map, std::vector;

#if __cpp_lib_ranges >= 201911L
using std::ranges::all_of, std::ranges::cbegin, std::ranges::cend, std::views::common, std::ranges::copy,
	std::ranges::count_if, std::ranges::find, std::views::split, std::views::transform;
#else
using nano::ranges::all_of, nano::ranges::cbegin, nano::ranges::cend, nano::views::common, nano::ranges::copy,
	nano::ranges::count_if, nano::ranges::find, nano::views::split, nano::views::transform;
#endif

using Passport = unordered_map<string, string>;

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
			throw runtime_error("parse error: missing value (no ':')");

		const auto ivalue = *it++ | common;
		const string value { cbegin(ivalue), cend(ivalue) };

		if (it != end)
			throw runtime_error("parse error: stray data in pair (extra ':')");

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

	inline auto parsePassport(istream& input)
	{
		Passport passport;

		string line;
		while (getline(input, line))
		{
			if (line.empty())
				return passport;

			parseLine(line, inserter(passport, passport.end()));
		}

		return passport;
	}

	inline auto parsePassports(istream& input)
	{
		vector<Passport> passports;

		while (input)
			passports.push_back(parsePassport(input));

		return passports;
	}
}

namespace Validator
{
	inline auto validateYear(const string& year, unsigned min, unsigned max)
	{
		if (year.size() != 4)
			// must be 4 characters
			return false;

		size_t end;
		const auto yearval = stoul(year, &end);

		if (end != year.size())
			// must be all digits
			return false;

		// must be between [min, max]
		return yearval >= min && yearval <= max;
	}

	inline auto validateHeight(const string& height)
	{
		size_t digitCount;
		const auto heightval = stoul(height, &digitCount);

		if (digitCount == 0)
			// must start with a number
			return false;

		auto unit = string_view { height };
		unit.remove_prefix(digitCount);
		if (unit == "cm")
			return heightval >= 150 && heightval <= 193;
		else if (unit == "in")
			return heightval >= 59 && heightval <= 76;

		// invalid unit
		return false;
	}

	inline auto validateHairColor(const string& hcl)
	{
		// must start with '#'
		if (hcl.size() == 0 || hcl.front() != '#')
			return false;

		// must be 6 hex digits
		auto hex = string_view { hcl };
		hex.remove_prefix(1);
		return hex.size() == 6 && all_of(hex, isxdigit);
	}

	inline auto validateEyeColor(const string& ecl)
	{
		constexpr auto colors = array {
			"amb", "blu", "brn", "gry", "grn", "hzl", "oth"
		};

		return find(colors, ecl) != cend(colors);
	}

	inline auto validatePassportId(const string& pid)
	{
		return pid.size() == 9 && all_of(pid, isdigit);
	}

	template <typename Func>
	inline auto validateField(const Passport& passport, const string& key, const Func& func)
	{
		const auto item = passport.find(key);
		if (item == passport.end())
			// field not found
			return false;

		return func(item->second);
	}

	inline auto validatePassport(const Passport& passport)
	{
		constexpr auto validators = array {
			pair { "byr", +[](const string& year) { return validateYear(year, 1920, 2002); }},
			pair { "iyr", +[](const string& year) { return validateYear(year, 2010, 2020); }},
			pair { "eyr", +[](const string& year) { return validateYear(year, 2020, 2030); }},
			pair { "hgt", validateHeight },
			pair { "hcl", validateHairColor },
			pair { "ecl", validateEyeColor },
			pair { "pid", validatePassportId }
		};

		return all_of(validators, [&](const auto& validator) {
			const auto [key, validate] = validator;
			return validateField(passport, key, validate);
		});
	}
}

int main()
{
	const auto passports = Parser::parsePassports(cin);
	const auto validCount = count_if(passports, Validator::validatePassport);

	cout << "There are " << validCount << " valid passports.\n";
}

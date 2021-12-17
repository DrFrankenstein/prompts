#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// unfortunately, we'll have to wait for C++23 before we can use std::views::zip :(
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

using std::cin, std::cout, std::runtime_error, std::size_t, std::string, std::string_view, std::vector;
using ranges::copy, ranges::istream_view, ranges::views::transform, ranges::views::zip;

class Diagnostics
{
public:
	Diagnostics() = default;

	Diagnostics(size_t size) : ones(size)
	{}

	auto ingest(string_view bits)
	{
		if (ones.empty())
			ones.resize(bits.size());

		if (bits.size() != ones.size())
			throw runtime_error { "inconsistent bit-length" };

		const auto newOnes = zip(ones, bits)
			| transform([](const auto tuple) {
				const auto [acc, bit] = tuple;
				return acc + static_cast<unsigned>(bit == '1');
			});

		copy(newOnes, ones.begin());
		++count;
	}

	[[nodiscard]]
	auto gamma() const
	{
		unsigned gamma = 0;
		const auto pivot = count / 2;
		for (const auto onecount : ones)
			gamma = gamma << 1 | static_cast<unsigned>(onecount > pivot);

		return gamma;
	}

	[[nodiscard]]
	auto epsilon() const
	{
		return ~gamma() & (1 << ones.size()) - 1;
	}

	[[nodiscard]]
	auto consumption() const
	{
		return gamma() * epsilon();
	}

private:
	vector<unsigned> ones;
	unsigned count = 0;
};

auto main() -> int
{
	Diagnostics diag;
	for (const auto& number : istream_view<string>(cin))
		diag.ingest(number);

	cout << diag.consumption() << '\n';
}

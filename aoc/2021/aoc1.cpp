#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>

#include <boost/circular_buffer.hpp>

using std::accumulate, std::cin, std::count_if, std::cout, std::istream_iterator, std::plus;
using boost::circular_buffer;

int main()
{
	istream_iterator<int> input {cin}, end;

	// I find this whole solution kinda clunky and inelegant, but I'm tired of it
	// so maybe I'll revisit it later...
	circular_buffer<int> window {3};
	window.push_back(*input++);
	window.push_back(*input++);
	window.push_back(*input++);

	auto lastSum = accumulate(window.begin(), window.end(), 0, plus{});

	const auto count = count_if(input, end, [&](const auto depth) {
		window.push_back(depth);
		const auto currSum = accumulate(window.begin(), window.end(), 0, plus{});
		const auto increased = currSum > lastSum;
		lastSum = currSum;
		return increased;
	});

	cout << count << '\n';
}

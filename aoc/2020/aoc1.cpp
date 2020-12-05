#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#if __cpp_lib_ranges >= 201911L
#include <ranges>
#else
#include <nanorange.hpp>
#endif

using std::back_inserter, std::copy, std::cin, std::cout,
      std::istream_iterator, std::pair, std::vector;

#if __cpp_lib_ranges >= 201911L
using std::ranges::cbegin, std::ranges::cend, std::ranges::subrange;
#else
using nano::cbegin, nano::cend, nano::subrange;
#endif

/** Generates k-combinations from a set, calling a functor for each.
 * @param k       size of the subsets to generate
 * @param set     set from which to take the subsets
 * @param reducer function that accumulates elements into a subset
 * @param functor function called with each complete subset
 * @param value   optional starting value to pass to the reducer
 *
 * @remarks This function has a complexity of O(C(n, k)), but stops when 'functor' returns true.
 */
template <typename Range, typename Reducer, typename Functor, typename Value>
void kcombinations(unsigned k, const Range& set, const Reducer& reducer, const Functor& functor, Value value = {})
{
	if (k == 0)
		return; // no 0-subsets for you.

	const auto end = cend(set);

	for (auto i = cbegin(set); i != end; ++i)
	{
		if (k > 1)
			kcombinations(k - 1, subrange(i + 1, end), reducer, functor, reducer(value, *i));
		else
			if (functor(reducer(value, *i)))
				return;
	}
}

pair<int, int> addmul(pair<int, int> left, int right)
{
	const auto [sum, product] = left;
	return { sum + right, product * right };
}

bool check(pair<int, int> values)
{
	constexpr int target = 2020;
	const auto [sum, product] = values;
	if (sum == target)
	{
		cout << product << '\n';
		return true;
	}

	return false;
}

int main()
{
	vector<int> entries;
	istream_iterator<int> in { cin };
	copy(in, {}, back_inserter(entries));

	cout << "with 2 entries: ";
	kcombinations(2, entries, addmul, check, pair{ 0, 1 });
	cout << "with 3 entries: ";
	kcombinations(3, entries, addmul, check, pair{ 0, 1 });
}

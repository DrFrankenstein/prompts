#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <ranges>
#include <utility>
#include <vector>

using std::abs, std::cin, std::cout, std::ranges::fold_left, std::istream,
	std::move, std::pair, std::plus, std::ranges::sort, std::vector,
	std::views::zip_transform;

template <typename T>
static auto readLists(istream& input)
{
	vector<T> leftList, rightList;
	while (input)
	{
		T left, right;
		cin >> left >> right;

		if (!input) break;

		leftList.push_back(left);
		rightList.push_back(right);
	}

	return pair{ move(leftList), move(rightList) };
}

static auto totalDistance(vector<int>& leftList, vector<int>& rightList)
{
	auto const distance = [](int left, int right) { return abs(left - right); };
	auto const distances = zip_transform(distance, leftList, rightList);
	return fold_left(distances, 0, plus<int>{});
}

int main()
{
	auto [leftList, rightList] = readLists<int>(cin);
	sort(leftList);
	sort(rightList);

	cout << totalDistance(leftList, rightList);
}

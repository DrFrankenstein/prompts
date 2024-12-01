#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

using std::abs, std::views::cartesian_product, std::cin, std::cout,
	std::ranges::fold_left, std::istream, std::move, std::pair, std::plus,
	std::ranges::sort, std::views::transform, std::vector,
	std::views::zip_transform, std::tuple;

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

static auto similarityScore(vector<int>& leftList, vector<int>& rightList)
{
	auto matches = cartesian_product(leftList, rightList)
		| transform([](tuple<int, int> values) {
			auto [left, right] = values;
			return left == right ? left : 0;
		});

	return fold_left(matches, 0, plus<int>{});
}

int main()
{
	auto [leftList, rightList] = readLists<int>(cin);
	sort(leftList);
	sort(rightList);

	cout << "Total distance: " << totalDistance(leftList, rightList) << '\n';
	cout << "Similarity score: " << similarityScore(leftList, rightList) << '\n';
}

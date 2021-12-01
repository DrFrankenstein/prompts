#include <algorithm>
#include <iostream>
#include <iterator>

using std::cin, std::count_if, std::cout, std::istream_iterator;

int main()
{
	int last;
	cin >> last;

	istream_iterator<int> depths {cin};
	const auto count = count_if(depths, {}, [&last](const auto curr) {
		bool deeper = curr > last;
		last = curr;
		return deeper;
	});

	cout << count << '\n';
}

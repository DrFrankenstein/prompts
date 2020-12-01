#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

using std::back_inserter, std::copy, std::cin, std::cout,
      std::istream_iterator, std::pair, std::vector;

template <typename Range, typename Func>
void combinations(const Range& r, const Func& f)
{
    auto end = r.end();

    for (auto i = r.begin(); i != end - 1; ++i)
        for (auto j = i + 1; j != end; ++j)
        {
            if(!f(*i, *j))
                return;
        }
}

template <typename Range, typename Value>
pair<Value, Value> findPairMatching(const Range& r, const Value target)
{
    pair<Value, Value> result;
    combinations(r, 
        [target,&result](const Value i, const Value j) {
            if (i + j == target)
            {
                result = {i, j};
                return false;
            }

            return true;
        }
    );

    return result;
}

int main()
{
    vector<int> values;
    istream_iterator<int> in { cin };
    copy(in, {}, back_inserter(values));

    auto [i, j] = findPairMatching(values, 2020);

    cout << i * j;
}

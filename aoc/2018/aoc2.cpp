#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <iostream>

using namespace std;

class FrequencyAnalyser
{
public:
    FrequencyAnalyser(string s)
    {
        for (char c : s)
            addChar(c);
    }

    bool findFrequency(unsigned freq)
    {
        auto begin = cbegin(counts);
        auto end = cend(counts);

        // we walk through the map to find the frequency we want.
        // I don't think that we'll see enough different letters to warrant
        // a bimap or something of the sort.
        auto it = find_if(begin, end, [&](auto item) {
            return item.second == freq;
        });

        return it != end;
    }

private:
    void addChar(char c)
    {
        ++counts[c];
    }

    map<char, unsigned> counts {};
};

int main()
{
    unsigned threes = 0;
    unsigned twos = 0;
    istream_iterator<string> ids (cin);

    for_each(ids, {}, [&](const string& id) {
        FrequencyAnalyser analyser (id);

        if (analyser.findFrequency(3))
            ++threes;
        if (analyser.findFrequency(2))
            ++twos;
    });

    unsigned checksum = twos * threes;
    cout << "Checksum is " << checksum << endl;
}

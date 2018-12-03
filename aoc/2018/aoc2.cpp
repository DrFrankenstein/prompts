#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <iostream>
#include <list>
#include <exception>

using namespace std;

class FrequencyAnalyser
{
public:
    FrequencyAnalyser(const string& s)
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

class BoxFinder
{
public:
    void addBox(const string& id)
    {
        if (found)
            return;

        for (const string& box : boxes)
            matchBoxes(id, box);

        boxes.push_back(id);
    }

private:
    void matchBoxes(const string& left, const string& right)
    {
        if (left.size() != right.size())
            throw logic_error("Invalid box ID length");

        auto begin = cbegin(left);
        auto end = cend(left);

        // first a first mismatch
        auto [ lmiddle, rmiddle ] = mismatch(begin, end, cbegin(right));

        if (mismatch(lmiddle + 1, end, rmiddle + 1).first == end)
        {	// no second mismatch found
            string common = string(begin, lmiddle).append(lmiddle + 1, end);
            cout << "Common letters between boxes: " << common << endl;
            found = true;
        }
    }

    list<string> boxes {};
    bool found { false };
};

int main()
{
    unsigned threes = 0;
    unsigned twos = 0;
    istream_iterator<string> ids (cin);

    BoxFinder finder;

    for_each(ids, {}, [&](const string& id) {
        FrequencyAnalyser analyser (id);

        if (analyser.findFrequency(3))
            ++threes;
        if (analyser.findFrequency(2))
            ++twos;

        finder.addBox(id);
    });

    unsigned checksum = twos * threes;
    cout << "Checksum is " << checksum << endl;
}

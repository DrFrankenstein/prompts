#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <unordered_set>

#include <boost/multi_array.hpp>

using namespace std;
using boost::multi_array;
using boost::extents;
using boost::indices;
using boost::multi_array_types::index_range;

struct Claim
{
    int id;
    unsigned x, y;
    unsigned width, length;
};

class ClaimParser
{
public:
    ClaimParser(istream& input)
        : input(input)
    { }

    bool readNext(Claim& claim)
    {
        if (!parseId(claim))
            return false;

        if (!parseLocation(claim))
            return false;

        if (!parseSize(claim))
            return false;

        return true;
    }

private:
    bool expect(char expected)
    {
        char actual;
        input >> actual;

        if (!input)
            return false;

        if (actual != expected)
        {
            cerr << "expected '" << expected << "' but got '" << actual << "'" << endl;
            return false;
        }

        return true;
    }

    bool parseId(Claim& claim)
    {
        if (!expect('#'))
            return false;

        if (!(input >> claim.id))
        {
            cerr << "error parsing id" << endl;
            return false;
        }

        return true;
    }

    bool parseLocation(Claim& claim)
    {
        if (!expect('@'))
            return false;

        if (!(input >> claim.x))
        {
            cerr << "error parsing x-location" << endl;
            return false;
        }

        if (!expect(','))
            return false;

        if (!(input >> claim.y))
        {
            cerr << "error parsing y-location" << endl;
            return false;
        }

        return true;
    }

    bool parseSize(Claim& claim)
    {
        if (!expect(':'))
            return false;

        if (!(input >> claim.width))
        {
            cerr << "error parsing width" << endl;
            return false;
        }

        if (!expect('x'))
            return false;

        if (!(input >> claim.length))
        {
            cerr << "error parsing length" << endl;
            return false;
        }

        return true;
    }
private:
    istream& input;
};

istream& operator >>(istream& input, Claim& claim)
{
    ClaimParser parser (input);
    if (!parser.readNext(claim))
        input.setstate(ios::failbit);

    return input;
}

template <typename T, unsigned FABRIC_WIDTH, unsigned FABRIC_LENGTH>
class FabricMapper
{
public:
    void addClaim(const Claim& claim)
    {
        auto scissors = indices[index_range(claim.y, claim.y + claim.length)][index_range(claim.x, claim.x + claim.width)];
        auto piece = map[scissors];
        for (auto row : piece)
            for (T& cell : row)
            {
                ++cell;
            }
    }

    long long conflicts() const
    {
        return accumulate(cbegin(map), cend(map), 0LL, [](auto count, auto row) {
            return count + count_if(cbegin(row), cend(row), [](T val) { return val > 1; });
        });
    }

private:
    multi_array<T, 2> map { extents[FABRIC_LENGTH][FABRIC_WIDTH] };
};

int main()
{
    auto start = chrono::high_resolution_clock::now();

    FabricMapper<unsigned short, 1000, 1000> mapper;

    istream_iterator<Claim> claims (cin);
    for_each(claims, {}, [&](const Claim& claim) { mapper.addClaim(claim); });

    cout << mapper.conflicts() << " square inches of fabric are in conflict." << endl;
    
    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Took " << time.count() << " µs." << endl;
}

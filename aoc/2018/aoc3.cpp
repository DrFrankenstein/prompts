#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>

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

    void readNext(Claim& claim)
    {
        expect('#');

        if (input.eof())
            return;

        parseId(claim);
        parseLocation(claim);
        parseSize(claim);
    }

    class ParseError : public runtime_error
    {
    public:
        ParseError(const string& message)
            : runtime_error(message)
        {}
    };

private:
    void expect(char expected)
    {
        char actual;
        input >> actual;

        if (input.eof())
            return;

        if (actual != expected)
        {
            ostringstream msg;
            msg << "expected '" << expected << "' but got '" << actual << "'";
            throw ParseError(msg.str());
        }
    }

    void parseId(Claim& claim)
    {
        if (!(input >> claim.id))
            throw ParseError("error parsing id");
    }

    void parseLocation(Claim& claim)
    {
        expect('@');

        if (!(input >> claim.x))
           throw ParseError("error parsing x-location");

        expect(',');

        if (!(input >> claim.y))
            throw ParseError("error parsing y-location");
    }

    void parseSize(Claim& claim)
    {
        expect(':');

        if (!(input >> claim.width))
            throw ParseError("error parsing width");

        expect('x');

        if (!(input >> claim.length))
            throw ParseError("error parsing length");
    }
private:
    istream& input;
};

istream& operator >>(istream& input, Claim& claim)
{
    ClaimParser parser (input);
    try
    {
        parser.readNext(claim);
    }
    catch (ClaimParser::ParseError& pe)
    {
        cerr << pe.what() << endl;
        input.setstate(ios::failbit);
    }

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

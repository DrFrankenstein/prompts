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

    // id = "#" number
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

    // location = "@" number "," number
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

    // size = ":" number "x" number
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

// Extracts and parses a claim from the input file.
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
    // Adds a claim to the map.
    void addClaim(const Claim& claim)
    {
        // cut the map down to the part that matches the claim
        auto scissors = indices[index_range(claim.y, claim.y + claim.length)][index_range(claim.x, claim.x + claim.width)];
        // this yields a "view" into the map that acts like another 2D array
        auto piece = map[scissors];

        // mark off all the cells in that view of the map
        bool clean = true;
        for (auto row : piece)
            for (T& cell : row)
                if (!claimCell(claim, cell))
                    clean = false;

        // if we didn't hit another claim, mark it clean (for now)
        if (clean)
            cleanClaims.insert(claim.id);
    }

    long long conflicts() const
    {  
        // count all the map cells that are marked as conflicting (-1)
        return accumulate(cbegin(map), cend(map), 0LL, [](auto count, auto row) {
            return count + count_if(cbegin(row), cend(row), [](T val) { return val == -1; });
        });
    }

    int cleanClaim() const
    {
        auto size = cleanClaims.size();
        if (size == 0)
        {
            cerr << "warning: all claims conflict." << endl;
            return 0;
        }

        if (size > 1)
            cerr << "warning: several non-conflicting claims." << endl;

        // we're only supposed to have one, so just return the first ID in the set
        return *cbegin(cleanClaims);
    }

private:
    bool claimCell(const Claim& claim, T& cell)
    {
        if (cell == 0)
        {   // no conflict, just reserve it with the claim ID
            cell = claim.id;
            return true;
        }

        // that cell's already taken, remove this claim ID from the clean set if it's in there
        auto elem = cleanClaims.find(cell);
        if (elem != end(cleanClaims))
            cleanClaims.erase(elem);

        // mark the cell as conflicting
        cell = -1;

        return false;
    }

    multi_array<T, 2> map { extents[FABRIC_LENGTH][FABRIC_WIDTH] };
    unordered_set<int> cleanClaims { };
};

int main()
{
    auto start = chrono::high_resolution_clock::now();

    // the story says "at least 1000 inches" but a glance through the input file seems to
    // place all the claims within 1000x1000; guess it'll crash if we're wrong
    FabricMapper<int, 1000, 1000> mapper;

    // go through all the claims in the standard input (cin) and add them to the map
    istream_iterator<Claim> claims (cin);
    for_each(claims, {}, [&](const Claim& claim) { mapper.addClaim(claim); });

    cout << mapper.conflicts() << " square inches of fabric are in conflict." << endl;
    cout << "Claim with id = " << mapper.cleanClaim() << " has no conflict." << endl;
    
    auto end = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Took " << time.count() << " µs." << endl;
}

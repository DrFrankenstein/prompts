#include <unordered_map>
#include <utility>
#include <iostream>

#include <boost/functional/hash.hpp>

using std::unordered_map;
using std::pair;
using std::make_pair;
using std::cin;
using std::cout;
using std::endl;
using std::swap;

using boost::hash;

typedef pair<int, int> coordinates;
typedef unordered_map<coordinates, int, hash<coordinates> > house_map;

int main()
{
    house_map map;
    coordinates coords (0, 0), robot_coords (0, 0);
    map.insert(make_pair(coords, 2));   // already visited (0, 0) at the start

    char c;
    while (cin >> c)
    {
        switch (c)
        {
        case '>':   ++coords.first; break;
        case '<':   --coords.first; break;
        case '^':   ++coords.second; break;
        case 'v':   --coords.second; break;
        default:    continue;
        }

        ++map[coords];

        swap(coords, robot_coords);
    }

    cout << map.size() << " houses got at least one present from Santa." << endl;
}

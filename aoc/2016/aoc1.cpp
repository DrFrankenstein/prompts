#include <iostream>
#include <set>
#include <cmath>
#include <locale>
#include <iterator>
#include <algorithm>

using namespace std;

using Coordinate = int;
using Distance = unsigned;

struct Position
{
    Coordinate x, y;

    Distance distanceFrom(const Position& other) const
    {
        auto dist = abs(x - other.x) + abs(y - other.y);
        return static_cast<Distance>(dist);
    }
};

bool operator <(const Position& left, const Position& right)
{
    if (left.y < right.y)
        return true;
    if (left.y == right.y && left.x < right.x)
        return true;

    return false;
}

enum class Direction { LEFT, RIGHT };

struct Instruction
{
    Direction direction;
    Distance distance;
};

istream& operator >>(istream& stream, Instruction& instruction)
{
    istream::sentry sentry (stream);

    if (!sentry)
        return stream;

    char direction;
    if (!stream.get(direction))
        return stream;

    switch (direction)
    {
    case 'L':
        instruction.direction = Direction::LEFT;
        break;
    case 'R':
        instruction.direction = Direction::RIGHT;
        break;
    default:
        stream.setstate(istream::failbit);
        return stream;
    }

    auto& numParser = use_facet<num_get<char> >(stream.getloc());
    auto state = stream.rdstate();
    numParser.get(stream, {}, stream, state, instruction.distance);
    stream.clear(state);

    if (stream.peek() == ',')
        stream.get();

    return stream;
}

class StreetGrid
{
public:
    bool addTrack(const Position& position)
    {
        return tracks.insert(position).second;
    }

private:
    set<Position> tracks {};
};

class Santa
{
public:
    Santa(StreetGrid& streetGrid)
        : streetGrid(streetGrid)
    {}

    void exec(const Instruction& instruction)
    {
        turn(instruction.direction);
        go(instruction.distance);
    }

    const Position& currentPosition() { return currentPos; }

private:
    enum Heading { NORTH, EAST, SOUTH, WEST };

    StreetGrid& streetGrid;
    Position currentPos { 0, 0 };
    Heading heading { NORTH };
    bool crossedTracks { false };

    void jump(Distance distance = 1)
    {
        Coordinate& coord = (heading & 1)? currentPos.x : currentPos.y;

        if (heading & 2)
            coord -= distance;
        else
            coord += distance;
    }

    void turn(Direction direction)
    {
        int headnum = heading;
        switch (direction)
        {
        case Direction::LEFT:
            --headnum;
            break;
        case Direction::RIGHT:
            ++headnum;
            break;
        }

        heading = static_cast<Heading>(headnum & 3);
    }

    void go(Distance distance)
    {
        while (!crossedTracks && distance--)
        {
            jump();	// well... step forward, anyway.
            if (!streetGrid.addTrack(currentPos))
            {
                crossedTracks = true;
                cout << "Easter Bunny HQ is " << currentPos.distanceFrom({0, 0}) << " blocks away." << endl;
            }
        }

        if (crossedTracks && distance)
        {
            jump(distance);
        }
    }
};

int main()
{
    StreetGrid streetGrid;
    Santa santa (streetGrid);

    istream_iterator<Instruction> instructions (cin);
    for_each(instructions, {}, [&](auto& instruction) { santa.exec(instruction); });

    cout << "Final destination is " << santa.currentPosition().distanceFrom({0, 0}) << " blocks away." << endl;
}

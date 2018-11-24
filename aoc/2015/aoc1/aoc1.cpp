#include <iostream>

using namespace std;

class Elevator
{
public:
    void up() { ++floornum; }
    void down() { --floornum; }
    int currentFloor() const { return floornum; }

private:
    int floornum {0};
};

class ElevatorAutomation
{
public:
    ElevatorAutomation(Elevator& elevator, istream& input)
        : elevator(elevator), input(input)
    {}

    void run()
    {
        char command;
        while (input.get(command))
            execCommand(command);
    }

    void execCommand(char command)
    {
        switch (command)
        {
        case '(':
            elevator.up();
            break;
        case ')':
            elevator.down();
            break;
        default:
            ;// do nothing
        }
    }

private:
    Elevator& elevator;
    istream& input;
};

int main()
{
    cin.exceptions(ios::badbit);

    Elevator elevator;
    ElevatorAutomation commands (elevator, cin);

    commands.run();

    cout << "Santa is on floor " << elevator.currentFloor() << endl;

    return 0;
}

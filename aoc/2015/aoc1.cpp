#include <iostream>

using namespace std;

class Elevator
{
public:
    void up() { ++floornum; }
    void down() { --floornum; }
    int currentFloor() const { return floornum; }
    bool inBasement() const { return floornum < 0; }

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
        {
            execCommand(command);
            checkBasement();
        }
    }

private:
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

    void checkBasement()
    {
        if (reachedBasement)
            return;

        if (elevator.inBasement())
        {
            reachedBasement = true;
            cout << "Elevator entered basement at command position " << input.tellg() << endl;
        }
    }

    Elevator& elevator;
    istream& input;
    bool reachedBasement {false};
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

#include <iterator>
#include <iostream>
#include <algorithm>
#include <set>
#include <vector>

using namespace std;

class ChronalCalibration
{
public:
    bool isCalibrated() const { return calibrated; }

    template <class It>
    void applyDrifts(It begin, It end)
    {
        all_of(begin, end, [this](int drift) { return applyDrift(drift); });

        if (!frequencyFound)
        {
            cout << "Resulting frequency: " << frequency << endl;
            frequencyFound = true;
        }
    }

private:
    bool applyDrift(int drift)
    {
        frequency += drift;
        checkCalibrated();

        return !frequencyFound || !calibrated;
    }

    void checkCalibrated()
    {
        if (!history.insert(frequency).second && !calibrated)
        {
            cout << "Calibrated with frequency " << frequency << endl;
            calibrated = true;
        }
    }

    int frequency { 0 };
    bool frequencyFound { false };
    bool calibrated { false };
    set<int> history { 0 };
};

int main()
{
    istream_iterator<int> input (cin);
    vector<int> frequencies;
    copy(input, {}, back_inserter(frequencies));

    ChronalCalibration calibration;

    while (!calibration.isCalibrated())
    {
        calibration.applyDrifts(begin(frequencies), end(frequencies));
    }
}

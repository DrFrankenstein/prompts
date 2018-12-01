#include <iterator>
#include <iostream>
#include <numeric>

using namespace std;

int main()
{
    // This reads in all numbers from 'cin', separated by whitespace
    istream_iterator<int> frequencies (cin);

    int result = accumulate(frequencies, {}, 0);

    cout << "Resulting frequency: " << result << endl;
}

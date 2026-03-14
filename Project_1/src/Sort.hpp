#include <iostream>

using namespace std;

class Sort {
public:
    Sort() {}
    ~Sort() {}

    void mergeSort(int[]);
    void quickSort(int[]);
    void introSort(int[]);

    void calculateAverage() const;
    void findMedian() const;
};
#include <iostream>

using namespace std;

class Solution {
public:
    Solution() {}
    ~Solution() {}

    void mergeSort(int[]);
    void quickSort(int[]);
    void introSort(int[]);

    void calculateAverage() const;
    void findMedian() const;
    void showMenu() const;
    void catchOptions() const;

    int getSize() const { return size;}
    void setSize(int newSize) { size = newSize; }
private:
    int size;
    int *tables[5];
};
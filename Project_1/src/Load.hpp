#include <iostream>

using namespace std;

// class Solution {
// public:
//     Solution() {}
//     ~Solution() {}

//     void mergeSort(int[]);
//     void quickSort(int[]);
//     void introSort(int[]);

//     void calculateAverage() const;
//     void findMedian() const;
//     void showMenu() const;
//     void catchOptions() const;  

//     int *getTable(int) const;
//     void setTable(int, int, int, int, int);
//     void setTableData(int*);
// private:
//     int *tables[5];
// };

void validDataGuard(int argc, char *argv[]);

class Load {
public:
    Load() {}
    ~Load() {}

    void loadData(int*) const;
    void filterData(int*) const;

    void showMenu() const;
    void catchOptions() const;

    void setTable(int, int, int, int, int);
    void setTableData(ifstream&) const;

    int **getTable(int) const;

private:
    int** tables[5];
};
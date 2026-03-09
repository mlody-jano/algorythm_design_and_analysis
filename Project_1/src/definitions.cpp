#include "declarations.hpp"

Solution::Solution() : size(0), tables{nullptr} {} // Constructor to initialize size and tables

Solution::~Solution() { // Destructor to clean up dynamically allocated memory
    for (int i = 0; i < 5; ++i) {
        delete[] tables[i];
    }
}

void Solution::mergeSort(int arr[]) { //Implement merge sort algorithm for data set
    // Implement merge sort algorithm
}

void Solution::quickSort(int arr[]) { // Implement quick sort algorithm for data set
    // Implement quick sort algorithm
}

void Solution::introSort(int arr[]) { // Implement intro sort algorithm for data set
    // Implement intro sort algorithm
}

void Solution::calculateAverage() const { // Implement calcuation of average of data
    // Implement average calculation
}

void Solution::findMedian() const { // Implement showing median of data 
    // Implement median calculation
}

void Solution::showMenu() const { // Implement menu display
    cout << "Menu:" << endl;
    cout << "1. Merge Sort - showcase of complexity" << endl;
    cout << "2. Exit" << endl;
}

void Solution::catchOptions() const { // Implement option handler 
    int choice;
    cin >> choice;
    switch(choice) {
        case 1:
            // Call mergeSort with a sample array
            break;
        case 2:
            exit(0);
        default:
            cout << "Invalid choice. Please provide a valid option." << endl;
    }
}

void Solution::setTable(int size1, int size2, int size3, int size4, int size5) { // Implement table initialization with given sizes
    tables[0] = new int[size1];
    tables[1] = new int[size2];
    tables[2] = new int[size3];
    tables[3] = new int[size4];
    tables[4] = new int[size5];
}

void validDataGuard(int argc, char *argv[]) { // Implement data validation for command line arguments
    if (argc != 6) {
        cout << "Usage: " << argv[0] << " <size1> <size2> <size3> <size4> <size5>" << endl;
        exit(1);
    }
    for (int i = 1; i <= 5; ++i) {
        if (atoi(argv[i]) <= 0) {
            cout << "All sizes must be positive integers." << endl;
            exit(1);
        }
    }
}
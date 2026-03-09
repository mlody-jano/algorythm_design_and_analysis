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
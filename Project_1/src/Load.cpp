#include "Load.hpp"
#include <fstream>

using namespace std;

Load::Load() {
    for (int i = 0; i < 5; ++i) {
        tables[i] = nullptr;
    }
}

Load::~Load() {
    for (int i = 0; i < 5; ++i) {
        delete[] tables[i];
    }
}

void Load::showMenu() const { // Implement menu display
    cout << "Menu:" << endl;
    cout << "1. Merge Sort - showcase of complexity" << endl;
    cout << "2. Exit" << endl;
}

void Load::catchOptions() const { // Implement option handler 
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

void Load::setTable(int size1, int size2, int size3, int size4, int size5) { // Implement table initialization with given sizes
    tables[0] = new int*[size1];
    tables[1] = new int*[size2];
    tables[2] = new int*[size3];
    tables[3] = new int*[size4];
    tables[4] = new int*[size5];
}

int **Load::getTable(int index) const { // Implement table retrieval by index
    if (index < 0 || index >= 5) {
        cerr << "Invalid table index." << endl;
        return nullptr;
    }
    return tables[index];
}

void Load::setTableData(ifstream &file) const{ // Implement setting data for a table

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return;
    }
    while (!file.eof()) {
    }
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
#include "DynamicTable.h"
/*
    Constructor for DynamicTable class, with initializing list.
    Initializes the capacity to 100, size to 0, and allocates memory for the table array.
*/
DynamicTable::DynamicTable() : capacity{100}, size{0}, table{new Ranking[capacity]} {}
/*
    Destructor for DynamicTable class.
    Frees the dynamically allocated memory to prevent memory leaks.
*/
DynamicTable::~DynamicTable() {
    delete[] table;
}
/*
    Adding an element of class Ranking at the beggining of the table.
*/
void DynamicTable::addElementAtBeginning(Ranking element) {
    checkCapacity();
    if (size == 0)
    { // If the table is empty, simply add the element at the first position
        table[0] = element;
        size++;
    }
    else if (size < capacity)
    {
        for (int i = size; i > 0; i--)
        {
            table[i] = table[i - 1];
        }
        table[0] = element;
        size++;
    }
}

/*
    Adding an element at the end of the table.
    Is O(1) due to doubling the capacity if table is full.
*/
void DynamicTable::addElementAtEnd(Ranking element) {
    checkCapacity();
    table[size] = element;
    size++;
}

/*
    Adding an element at a specific position in the table.
*/
void DynamicTable::addElementAtPosition(Ranking element, int position) {
    if (position < 0 || position > capacity)
    {
        cerr << "Invalid position. Element not added." << endl;
        return;
    }
    checkCapacity();

    for (int i = capacity - 1; i > position; i--)
    { // Shifting elements on the right of position to make room for new element
        table[i] = table[i - 1];
    }
    table[position] = element;
    size++;
}

/*
    Deleting an element from the beginning of the table.
*/
void DynamicTable::deleteElementAtBeginning() {
    if (size == 0)
    {
        cerr << "Table is empty. No element to delete." << endl;
        return;
    }
    for (int i = 0; i < size - 1; i++)
    { // Shifting elements on the right to fill the gap left by deleted element
        table[i] = table[i + 1];
    }
    size--;
}

/*
    Deleting an element from the end of the table.
*/
void DynamicTable::deleteElementAtEnd() {
    if (size == 0)
    {
        cerr << "Table is empty. No element to delete." << endl;
        return;
    }
    table[size - 1].setName(""); // Deleting last element is O(1) due to having the size variable
    table[size - 1].setScore(0);
    size--;
}

/*
    Deleting an element from a specific position in the table.
*/
void DynamicTable::deleteElementAtPosition(int position) {
    if (position < 0 || position >= size)
    {
        cerr << "Invalid position. No element deleted." << endl;
        return;
    }
    for (int i = position; i < size - 1; i++)
    { // Shifting elements on the right of position to fill the gap left by deleted element
        table[i] = table[i + 1];
    }
    size--;
}

/*
    Searching for an element in the table.
    For debug purposes, might print the position of element.
*/
volatile bool DynamicTable::searchElement(Ranking element) const {
    bool found = false;
    for (int i = 0; i < size; i++)
    { // Linear search through the table
        if (table[i].getScore() == element.getScore())
        {
            // cout << "Element " << element << " found at position " << i << "." << endl;
            found = true;
            // Do not break; continue searching the whole array
        }
    }
    if (!found)
    {
        cout << "Element " << element.getName() << " not found in the table." << endl;
    }
    return found;
}

/*
    Checking the capacity of the table and doubling it if necessary.
    Doubles the capacity to prevent frequent copying.
*/
void DynamicTable::checkCapacity() {
    if (size == capacity)
    {
        Ranking *newTable = new Ranking[capacity * 2];
        for (int i = 0; i < size; i++)
        {
            newTable[i] = table[i];
        }
        delete[] table;
        table = newTable;
        capacity *= 2;
    }
}

/*
    Displaying the contents of the table for testing purposes.
*/
void DynamicTable::display() const {
    cout << "Table contents: " << endl;
    for (int i = 0; i < size; i++)
    {
        cout << i << " ";
    }
    cout << endl;

    for (int i = 0; i < size; i++)
    {
        table[i].display();
        cout << " ";
    }
    cout << endl;
}

/* 
    Returning an element at a specific position in the table.
*/
Ranking DynamicTable::returnElementAtPosition(int position) const {
        if (position < 0 || position >= size)
        {
            cerr << "Invalid position. No element returned." << endl;
            return Ranking(); // Return default constructed Ranking object to indicate an error
        }
        return table[position];
}

/*
    QuickSort function using three-way partitioning to handle duplicates efficiently. 
    Three way partitioning is used to improve performance due to many duplicate records being present in dataset.
*/
void DynamicTable::quickSort(int low, int high) {
    if (low >= high) return;

    // Three-way partition — handles duplicates in O(n) per level
    int pivot = table[low + (high - low) / 2].getScore();
    int lt = low;    // table[low..lt-1]  < pivot
    int gt = high;   // table[gt+1..high] > pivot
    int i  = low;    // table[lt..i-1]   == pivot

    while (i <= gt) {
        int score = table[i].getScore();
        if      (score < pivot) swap(table[lt++], table[i++]);
        else if (score > pivot) swap(table[i],    table[gt--]);
        else                    i++;
    }
    // Now: table[low..lt-1] < pivot, table[lt..gt] == pivot, table[gt+1..high] > pivot
    // The entire [lt..gt] range is already in its final position — skip it entirely
    quickSort(low,   lt - 1);
    quickSort(gt + 1, high);
}

/*
    MergeSort function using a temporary buffer to improve performance by avoiding multiple memory allocations.
    The buffer is allocated once at the start and is used in all recursive calls, passed via pointer.
*/
void DynamicTable::mergeSort(int left, int right) {
    if (left >= right) return;
    Ranking* buffer = new Ranking[size];                        // Allocate the buffer once
    mergeSortInternal(left, right, buffer);                     // Pass the buffer via pointer to recursive calls
    delete[] buffer;                                            // Free the buffer after sorting is complete 
}

/*
    Private mergeSort function that performs the actual merge sort, encapsulated to be used with the buffer.
    Such implementation ensures that we cannot accidentally call mergeSort without the buffer, which coulc lead to memory leaks of poor performance.
*/
void DynamicTable::mergeSortInternal(int left, int right, Ranking* buffer) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortInternal(left, mid, buffer);
    mergeSortInternal(mid + 1, right, buffer);
    merge(left, mid, right, buffer);
}
/*
    IntroSort function — uses quicksort for the main sorting, but switches to heapsort or insertion sort.
    Heapsort is used when the recursive depth exceeds a certain limit, which might indicate that quicksort is falling into O(n^2).
    Insertion sort is used for small partitions to optimize performance, as it has lower overhead than quicksort for small arrays.
*/
void DynamicTable::introSort(int low, int high, int depthLimit) {
    while (low < high) {
        if (high - low + 1 <= 16) {
            insertionSort(low, high);
            return;
        }
        if (depthLimit == 0) {                                          // If the recursion calls exceed the depth limit, switch to heapsort
            heapSort(low, high);
            return;
        }

        int pivot = table[low + (high - low) / 2].getScore();           // Chooses the middle element using the median of first middle and last element
        int lt = low, gt = high, i = low;
        while (i <= gt) {
            int score = table[i].getScore();
            if      (score < pivot) swap(table[lt++], table[i++]);
            else if (score > pivot) swap(table[i],    table[gt--]);
            else                    i++;
        }

        if (lt - low < high - gt) {
            introSort(low,    lt - 1, depthLimit - 1);
            low = gt + 1;
        } else {
            introSort(gt + 1, high,   depthLimit - 1);
            high = lt - 1;
        }
    }
}

/*
    InsertionSort function — sorts the elements in the range [low, high] using insertion sort.
*/
void DynamicTable::insertionSort(int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        Ranking key = table[i];
        int j = i - 1;
        while (j >= low && table[j].getScore() > key.getScore()) {
            table[j + 1] = table[j];
            j--;
        }
        table[j + 1] = key;
    }
}

/* 
    Merge function using the buffer passed via pointer to reduce recursive momery allocations.
*/
void DynamicTable::merge(int left, int mid, int right, Ranking* buffer) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    for (int i = 0; i < n1; i++) buffer[i]      = table[left + i];
    for (int j = 0; j < n2; j++) buffer[n1 + j] = table[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (buffer[i].getScore() <= buffer[n1 + j].getScore())
            table[k++] = buffer[i++];
        else
            table[k++] = buffer[n1 + j++];
    }
    while (i < n1) table[k++] = buffer[i++];
    while (j < n2) table[k++] = buffer[n1 + j++];
}
/*
    HeapSort function — uses a binary heap to sort the elements.
    It first builds a max heap from the input array, then repeatedly extracts the maximum element and rebuilds the heap.
*/
void DynamicTable::heapSort(int low, int high) {
    // Implementation of heap sort algorithm
    int n = high - low + 1;

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(low, high, i);                                              // Build heap

    // One by one extract elements from heap
    for (int i = n - 1; i > 0; i--) {
        swap(table[low], table[low + i]);                                   // Move current root to end
        heapify(low, low + i - 1, 0);                                       // Call heapify on the reduced heap
    }
}
/*
    Heapify function — maintains the heap property for a subtree rooted at index root, which is an index in the relative range [low, high].
     The function assumes that the binary trees rooted at left and right child of root are already heaps
*/
void DynamicTable::heapify(int low, int high, int root) {
    int largest = root;
    int left  = 2 * root + 1;
    int right = 2 * root + 2;
    int n = high - low + 1;   // ✅ work in relative indices

    if (left < n && table[low + left].getScore() > table[low + largest].getScore())
        largest = left;

    if (right < n && table[low + right].getScore() > table[low + largest].getScore())
        largest = right;

    if (largest != root) {
        swap(table[low + root], table[low + largest]);
        heapify(low, high, largest);
    }
}

/*
    Function to find the median value of the scores in the table. Assumes the table is sorted by score.
*/
float DynamicTable::findMedian() const {
    return table[size / 2].getScore(); // Assuming the table is sorted and we want the median score of the rankings
}

/**
    Function to find the average value of the scores in the table.
*/
float DynamicTable::findAverage() const {
    if (size == 0) {
        cerr << "Table is empty. Cannot calculate average." << endl;
        return 0; // Return 0 to indicate an error
    }
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += table[i].getScore(); // Assuming we want to average the scores of the rankings
    }
    return static_cast<float>(sum) / size; // Return the average score
}
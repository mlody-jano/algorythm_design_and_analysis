#ifndef DYNAMICTABLE_H
#define DYNAMICTABLE_H
#include "Ranking.h"

#include <iostream>

#pragma once

using namespace std;

class DynamicTable
{
public:
    DynamicTable();
    ~DynamicTable();

    inline int returnSize() const { return size; };
    inline int returnCapacity() const { return capacity; };

    void addElementAtBeginning(Ranking);
    void addElementAtEnd(Ranking);
    void addElementAtPosition(Ranking, int);

    void deleteElementAtBeginning();
    void deleteElementAtEnd();
    void deleteElementAtPosition(int);

    volatile bool searchElement(Ranking) const;

    Ranking returnElementAtPosition(int position) const;

    void checkCapacity();

    void display() const; // Utility function to print the contents of the table for testing purposes

    float findAverage() const;
    float findMedian() const;

    void quickSort(int low, int high);
    void mergeSort(int left, int right);
    void introSort(int low, int high, int depthLimit);
    void heapSort(int low, int high);

private:
    int capacity;
    int size;
    Ranking *table;

    int partition(int low, int high);
    void merge(int left, int mid, int right, Ranking*);
    void mergeSortInternal(int left, int right, Ranking*);
    void insertionSort(int low, int high);
    // void heapSort(int low, int high);
    void heapify(int low, int high, int root);
};

#endif
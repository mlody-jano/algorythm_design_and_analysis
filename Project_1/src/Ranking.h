#ifndef RANKING_H
#define RANKING_H
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

class Ranking {
private:
    string name;
    int score;
public:
    Ranking();
    Ranking(const string, int);
    ~Ranking();
    string getName() const;
    int getScore() const;

    string setName(string);
    int setScore(int);

    void display() const;
};
#endif
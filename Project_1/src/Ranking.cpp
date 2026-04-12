#include "Ranking.h"
#include <iostream>

Ranking::Ranking() : name(""), score(0) {}

Ranking::Ranking(const string name, int score) : name(name), score(score) {}

Ranking::~Ranking() {}

string Ranking::getName() const {
    return name;
}

int Ranking::getScore() const {
    return score;
}

string Ranking::setName(string newName) {
    name = newName;
    return name;
}

int Ranking::setScore(int newScore) {
    score = newScore;
    return score;
}

void Ranking::display() const {
    cout << "Name: " << name << ", Score: " << score << endl;
}
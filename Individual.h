#pragma once
#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include <vector>
using namespace std;

class Individual {
public:
    vector<int> genome;
    int cost;

    Individual(vector<int> g, int c) {
        this->genome = g;
        this->cost = c;
    }

    bool operator<(const Individual& other) const {
        // Sort by min cost, using the priority queue
        return cost < other.cost;
    }
};

#endif INDIVIDUAL_H
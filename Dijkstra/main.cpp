#pragma once

#include <iostream>
#include "Graph.cpp"
#include "Dijkstra.cpp"
#include <chrono>

using namespace std;

double execute(int type, int size) {
    auto graph = new Graph(size);
    auto dijkstra = new Dijkstra(graph, 0);

    auto start = std::chrono::high_resolution_clock::now();
    dijkstra->perform(type);
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(finish - start).count();
    return duration;
}

int main() {
    int size = 10;
    int type = 0;

    int executeCount = 100;
    double averageTime = 0;

    for (int i = 0; i < executeCount; ++i) {
        averageTime += execute(size, type);
    }

    averageTime = averageTime / executeCount;
    cout << averageTime;
    return 0;
}

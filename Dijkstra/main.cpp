#pragma once

#include <iostream>
#include "Graph.cpp"
#include "Dijkstra.cpp"

using namespace std;

int main() {

    auto graph = new Graph("Graph5.txt");
    auto dijkstra = new Dijkstra(graph, 0);
    auto result = dijkstra->performSerial();
    return 0;
}

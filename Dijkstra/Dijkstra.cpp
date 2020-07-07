#pragma once

#include <limits>
#include "Graph.cpp"

using namespace std;

const int INFINITE = numeric_limits<int>::max();

class Dijkstra {
public:
    Graph *graph;

    bool *visited; //S
    int *distance; //D
    int source;
    int n;

    Dijkstra(Graph *graph, int source, int n) : graph(graph), source(source), n(n) {
        visited = new bool[n];
        distance = new int[n];
        for (int i = 0; i < n; ++i) {
            visited[i] = false;
            distance[i] = INFINITE;
        }
    }

    void performSerial() {
        visited[source] = true;
        for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
            if (vertex == source) {
                continue;
            }
            distance[vertex] = graph->getDistance(source, vertex);
        }
        for (int i = 0; i < graph->vertexes - 2; ++i) {
            int currentVertex = findMinDistance();
            visited[currentVertex] = true;
            for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                distance[vertex] = min(distance[vertex], distance[currentVertex] + graph->getDistance(currentVertex, vertex));
            }
        }
    }

private:

    int findMinDistance() const {
        int minDistance = INFINITE, minVertex;
        for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
            if (visited[vertex]) {
                continue;
            }
            if (distance[vertex] < minDistance) {
                minDistance = distance[vertex];
                minVertex = vertex;
            }
        }
        return minVertex;
    }
};
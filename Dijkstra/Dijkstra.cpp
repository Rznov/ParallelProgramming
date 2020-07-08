#pragma once

#include "Graph.cpp"

using namespace std;

const int INFINITE = 10000;

class Dijkstra {
public:
    Graph *graph;

    bool *visited; //S
    int *distance; //D
    int source;
    int n;

    Dijkstra(Graph *graph, int source) : graph(graph), source(source), n(graph->vertexes) {
        visited = new bool[n];
        distance = new int[n];
        for (int i = 0; i < n; ++i) {
            visited[i] = false;
            distance[i] = INFINITE;
        }
    }

    int *perform(int type) {
        visited[source] = true;
        distance[source] = 0;
#pragma omp parallel for schedule(static) default(shared)
        for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
            if (vertex == source) {
                continue;
            }
            distance[vertex] = graph->getDistance(source, vertex);
        }
        for (int i = 0; i < graph->vertexes - 1; ++i) {
            int currentVertex = findMinDistance();
            visited[currentVertex] = true;
            forLoop(type, currentVertex);
        }
        return distance;
    }

private:
    void forLoop(int type, int currentVertex) {
        switch (type) {
            case 0: {
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex],
                                           distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
            case 1: {
#pragma omp parallel for schedule(static) default(shared)
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex],
                                           distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
            case 2: {
#pragma omp parallel for schedule(dynamic) default(shared)
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex],
                                           distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
            case 3: {
#pragma omp parallel for schedule(guided) default(shared)
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex], distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
        }
    }

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
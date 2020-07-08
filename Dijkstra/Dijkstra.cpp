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
#pragma omp parallel for schedule(static)
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
        log(toString(distance, n));
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
#pragma omp parallel for schedule(static)
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex],
                                           distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
            case 2: {
#pragma omp parallel for schedule(guided)
                for (int vertex = 0; vertex < graph->vertexes; ++vertex) {
                    if (visited[vertex]) {
                        continue;
                    }
                    distance[vertex] = min(distance[vertex],
                                           distance[currentVertex] + graph->getDistance(currentVertex, vertex));
                }
            }
            case 3: {
#pragma omp parallel for schedule(dynamic)
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

    void log(const string &message) {
        ofstream stream(
                "qs." + to_string(1) + "-" + to_string(2) + ".log",
                ios_base::app);
        stream << time(nullptr) << " - " << message << endl;
        stream.close();
    }

    static string toString(int *array, int size) {
        string contentString;
        if (size < 20) {
            for (int i = 0; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        } else {
            for (int i = 0; i < 5; i++) {
                contentString += to_string(array[i]) + " ";
            }
            contentString += " ... ";
            for (int i = size - 5; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        }
        return "|" + to_string(size) + "|[" + contentString + "]";
    }
};
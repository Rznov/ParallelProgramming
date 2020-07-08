#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

class Graph {
public:
    const int INFINITE = 10000;

    int *graph;
    int vertexes; // vertexes
    int edges; // edges

    string filename;

    Graph(int vertexes) : vertexes(vertexes) {
        graph = new int [vertexes * vertexes];
        fill();
        string name = "Graph";
        filename = name + to_string(vertexes) + ".txt";
        writeToFile();
    }

    Graph(string filename) {
        ifstream file(filename);
        file >> vertexes;
        graph = new int [vertexes * vertexes];
        for (size_t i = 0; i < vertexes; ++i) {
            for (size_t j = 0; j < vertexes; ++j) {
                file >> graph[i * vertexes + j];
            }
        }
        file.close();
    }

    bool writeToFile() const {
        if (vertexes == 0) {
            return false;
        }
        ofstream file(filename);
        file << vertexes << endl;
        for (size_t i = 0; i != vertexes; ++i) {
            for (size_t j = 0; j != vertexes; ++j) {
                file << graph[i * vertexes + j] << " ";
            }
            file << endl;
        }
        file.close();
        return true;
    }

    int getDistance(int source, int vertex) {
        return graph[source * vertexes + vertex];
    }

private:
    void fill() {
        if (vertexes > 0) {
            for (int i = 0; i < vertexes; ++i) {
                for (int j = 0; j < vertexes; ++j) {
                    if (i == j) {
                        graph[i * vertexes + j] = 0;
                        continue;
                    }
                    if (rand() % 2) {
                        graph[i * vertexes + j] = rand() % 10;
                    } else {
                        graph[i * vertexes + j] = INFINITE;
                    }
                }
            }
        }
    }
};

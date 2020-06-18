#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

class Matrix {
public:
    size_t rows;
    size_t columns;
    double *values;
    string filename;

    Matrix() : Matrix(0, 0) {}

    Matrix(size_t rows, size_t columns) : rows(rows), columns(columns) {
        values = new double [rows * columns];
        string name = "Matrix";
        string localFilename = name + to_string(rows) + "by" + to_string(columns) + ".txt";
        filename = localFilename;
    }

    Matrix(string filename) {
        ifstream file(filename);
        file >> rows >> columns;
        values = new double [rows * columns];
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                file >> values[i * columns + j];
            }
        }
        file.close();
    }

    bool fillRandom() const {
        if (rows == 0) {
            return false;
        }
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                values[i * columns + j] = rand() % 3 + 1;
            }
        }
        return true;
    }

    bool fillLinearSystem() {
        if (rows == 0) {
            return false;
        }
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                if (i == j) {
                    values[i * columns + j] = 3 * columns + rand() % (5 * columns - 3 * columns);
                } else {
                    values[i * columns + j] = rand() % 3;
                }
            }
        }
        return true;
    }

    bool writeToFile() const {
        if (rows == 0) {
            return false;
        }
        ofstream file(filename);
        file << rows << " " << columns << endl;
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                file << values[i * columns + j] << " ";
            }
            file << endl;
        }
        file.close();
        return true;
    }

    void writeToConsole() const {
        cout << rows << " " << columns << endl;
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                cout << values[i * columns + j] << " ";
            }
            cout << endl;
        }
    }

    ~Matrix() {
        delete [] values;
    }
};

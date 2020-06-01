#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

class Matrix {
public:
    size_t rows;
    size_t columns;
    double * values;

    Matrix() : Matrix(0, 0) {}

    Matrix(size_t rows, size_t columns) : rows(rows), columns(columns) {
        values = new double [rows * columns];
    }

    Matrix(string filename) {
        ifstream file(filename);
        file >> rows >> columns;
        values = new double [rows * columns];
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                file >> values[i * rows + j];
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
                values[i * rows + j] = rand() % 3 + 1;
            }
        }
        return true;
    }

    bool fillLinearSystem() {
        if (rows == 0) {
            return false;
        }
        srand(time(0));
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                if (i == j) {
                    values[i * rows + j] = 3 * columns + rand() % (5 * columns - 3 * columns);
                } else {
                    values[i * rows + j] = rand() % 3;
                }
            }
        }
        return true;
    }

    bool writeToFile() const {
        if (rows == 0) {
            return false;
        }
        string name = "Matrix";
        string filename = name + to_string(rows) + "by" + to_string(columns) + ".txt";
        ofstream file(filename);
        file << rows << " " << columns << endl;
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                file << values[i * rows + j] << " ";
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
                cout << values[i * rows + j] << " ";
            }
            cout << endl;
        }
    }

    ~Matrix() {
        delete [] values;
    }
};

#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

class Matrix {
public:
    size_t rows;
    size_t columns;
    int ** values;

    Matrix() : Matrix(0, 0) {}

    Matrix(size_t rows, size_t columns) : rows(rows), columns(columns) {
        values = new int * [rows];
        values[0] = new int [rows * columns];
        for (size_t i = 1; i != rows; ++i) {
            values[i] = new int[columns];
        }
    }

    Matrix(string filename) {
        ifstream file(filename);
        file >> rows >> columns;
        values = new int * [rows];
        values[0] = new int [rows * columns];
        for (size_t i = 0; i != rows; ++i) {
            for (size_t j = 0; j != columns; ++j) {
                file >> values[i][j];
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
                values[i][j] = rand() % 10;
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
                file << values[i][j] << " ";
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
                cout << values[i][j] << " ";
            }
            cout << endl;
        }
    }

    ~Matrix() {
        delete [] values[0];
        delete [] values;
    }
};
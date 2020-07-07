#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class IntArray {
public:
    int size;
    int *values;
    string filename;

    IntArray(int size) : size(size) {
        values = new int [size];
        string name = "Array";
        filename = name + to_string(size);
        if (fillRandom()) {
            writeToFile();
        }
    }

    IntArray(string filename) {
        ifstream file(filename);
        file >> size;
        values = new int [size];
        for (int i = 0; i < size; ++size) {
            file >> values[i];
        }
        file.close();
    }

    IntArray(const int *inputValues, int size) : size(size) {
        values = new int [size];
        for (int i = 0; i < size; ++i) {
            values[i] = inputValues[i];
        }
    }

    IntArray(IntArray *leftPart, IntArray *rightPart) : size(leftPart->size + rightPart->size) {
        values = new int[size];
        for (int i = 0; i < leftPart->size; ++i) {
            values[i] = leftPart->values[i];
        }
        for (int i = leftPart->size; i < size; ++i) {
            values[i] = rightPart->values[i - leftPart->size];
        }
    }

    IntArray(const int *from, int start, int finish) {
        size = finish - start - 1;
        values = new int [size];
        for (int i = start; i < finish; ++i) {
            values[i - start] = from[i];
        }
    }

    ~IntArray() {
        delete [] values;
    }

    int getPivot() const {
        int start = values[0];
        int end = values[size - 1];
        int middle = values[(start + end) / 2];
        if (start <= middle && middle <= end) {
            return middle;
        } else if (middle <= start && start <= end) {
            return start;
        } else {
            return end;
        }
    }

    IntArray *getPart(int processId, int processesNum) const {
        int partSize = size / processesNum;
        int start = processId * partSize;
        if (processId == processesNum - 1) {
            partSize = size - start;
        }
        return new IntArray(&values[start], partSize);
    }

    void separate(int pivot, IntArray **leftPart, IntArray **rightPart) const {
        int left = 0, right = size - 1;
        while (true) {
            do {
                ++left;

            } while (values[left] < pivot);
            do {
                --right;

            } while (values[right] >= pivot);
            if (left >= right) {
                break;
            }
            swap(values[left], values[right]);
        }
        *leftPart = new IntArray(values, left);
        *rightPart = new IntArray(&values[left], size - left);
    }

    bool fillRandom() const {
        if (size == 0) {
            return false;
        }
        for (int i = 0; i < size; ++i) {
            values[i] = rand() % 30;
        }
        return true;
    }

    bool writeToFile() const {
        if (size == 0) {
            return false;
        }
        ofstream file(filename);
        file << size << endl;
        for (int i = 0; i < size; ++i) {
            file << values[i] << " ";
        }
        file.close();
        return true;
    }

    string toString() const {
        string contentString;
        if (size < 20) {
            for (int i = 0; i < size; i++) {
                contentString += to_string(values[i]);
                if (i < size - 1) contentString += " ";
            }
        } else {
            for (int i = 0; i < 5; i++) {
                contentString += to_string(values[i]) + " ";
            }
            contentString += " ... ";
            for (int i = size - 5; i < size; i++) {
                contentString += to_string(values[i]);
                if (i < size - 1) contentString += " ";
            }
        }
        return "|" + to_string(size) + "|[" + contentString + "]";
    }

private:
    int findIdx(int element) {
        for (int i = 0; i < size; ++size) {
            if (values[i] == element) {
                return i;
            }
        }
    }
};
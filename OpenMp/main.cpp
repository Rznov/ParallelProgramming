#include <iostream>
#include <chrono>
#include "Matrix.cpp"
#include "Multiplication.cpp"

using namespace std;

double execute(int rows, int columns, int type) {

    auto A = new Matrix(rows, columns);
    auto B = new Matrix(rows,columns);
    A->fillRandom();
    B->fillRandom();

    Multiplication * multiplication;
    switch (type) {
        case 0:
            multiplication = new Multiplication(A, B);
            break;
        case 1:
            multiplication = new ParallelForStaticScheduleMultiplication(A, B);
            break;
        case 2:
            multiplication = new ParallelForDynamicScheduleMultiplication(A, B);
            break;
        case 3:
            multiplication = new ParallelForGuidedScheduleMultiplication(A,B);
            break;
    }

    auto start = std::chrono::high_resolution_clock::now();
    multiplication->multiply();
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(finish - start).count();
    return duration;
}

int main() {
    int rows = 500;
    int columns = 500;
    int type = 1;
    int executeCount = 100;
    double averageTime = 0;

    for (int i = 0; i < executeCount; ++i) {
        averageTime += execute(rows, columns, type);
    }

    averageTime = averageTime / executeCount;
    cout << averageTime;

    return 0;
}

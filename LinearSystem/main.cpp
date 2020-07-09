#include <iostream>
#include <mpi.h>
#include <chrono>
#include "Matrix.cpp"
#include "JacobiSolver.cpp"

using namespace std;

void timeToFile(double time) {
    ofstream stream("time.txt");
    stream << time << endl;
    stream.close();
}

double solveSystem(int size) {
    auto A = new Matrix(size, size + 1);
    A->fillLinearSystem();
    A->writeToFile();
    auto x = new Matrix(size, 1);
    x->fillRandom();
    x->writeToFile();

    string coefficients = A->filename;
    string estimate = x->filename;
    double error = 0.01;

    auto solver = new JacobiSolver();
    int processId = solver->processId;
    auto start = std::chrono::high_resolution_clock::now();
    solver->solve(coefficients, estimate, error);
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(finish - start).count();
    //timeToFile(duration);

        return (duration);

}

int main() {

    int size = 10;
    int iterations = 10;
    double averageTime = 0;
    MPI_Init(nullptr, nullptr);
    for (int i = 0; i != iterations; ++i) {
        averageTime += solveSystem(size);
    }
    MPI_Finalize();
    averageTime = averageTime / iterations;
    timeToFile(averageTime);

    return 0;
}
#include <iostream>
#include <mpi.h>
#include "Matrix.cpp"
#include "JacobiSolver.cpp"

using namespace std;

int main() {

    int size = 3;
    auto A = new Matrix(size, size + 1);
    A->fillLinearSystem();
    A->writeToFile();
    auto x = new Matrix(size, 1);
    x->fillRandom();
    x->writeToFile();

    string coefficients = A->filename;
    string estimate = x->filename;
    double error = 0.01;

    MPI_Init(nullptr, nullptr);
    auto solver = new JacobiSolver();
    solver->solve(coefficients, estimate, error);
    MPI_Finalize();

    return 0;
}

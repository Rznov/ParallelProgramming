#include <iostream>
#include <mpi.h>
#include "Matrix.cpp"
#include "JacobiSolver.cpp"

using namespace std;

int main() {

    int size = 3;
    auto A = new Matrix(size, size);
    srand(time(0));
    A->fillLinearSystem();
    auto x = new Matrix(size, 1);
    x->fillRandom();
    A->writeToConsole();
//    x->writeToConsole();
    auto solver = new JacobiSolver(A, x->values, 0.01);
    solver->solve();

    return 0;
}

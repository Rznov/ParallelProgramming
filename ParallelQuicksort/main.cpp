#include <iostream>
#include "IntArray.cpp"
#include "Quicksort.cpp"
#include <mpi.h>

int main() {

    MPI_Init(nullptr, nullptr);
    int size = 10;
    auto globalArray = new IntArray(size);
    auto sort = Quicksort(globalArray->filename);
    sort.sort();
    MPI_Finalize();

    return 0;
}

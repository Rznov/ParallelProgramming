#include <iostream>
#include "IntArray.cpp"
#include "Quicksort.cpp"
#include <mpi.h>

void timeToFile(double time) {
    ofstream stream("time.txt");
    stream << time << endl;
    stream.close();
}

int main() {

    MPI_Init(nullptr, nullptr);
    int size = 1e7;

    auto globalArray = new IntArray(size);
    auto sort = Quicksort(globalArray->filename);

    auto start = std::chrono::high_resolution_clock::now();
    sort.sort();
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(finish - start).count();
    timeToFile(duration);
    MPI_Finalize();

    return 0;
}

#pragma once
#include <mpi.h>

class ProcessGroup {
public:
    MPI_Comm communicator;
    int processId;
    int processesNum;

    ProcessGroup(MPI_Comm communicator) : communicator(communicator) {
        MPI_Comm_rank(communicator, &processId);
        MPI_Comm_size(communicator, &processesNum);
    }

    bool isRight() const {
        return processId >= processesNum / 2;
    }

    int getNeighbour() const {
        if (isRight()) {
            return processId - (processesNum + 1) / 2;
        }
        return processId + (processesNum + 1) / 2;
    }
};


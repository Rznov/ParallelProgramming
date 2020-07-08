#include <mpi.h>
#include "IntArray.cpp"
#include "ProcessGroup.cpp"

using namespace std;

const int MAIN_PROCESS = 0;
const int MAX_ARRAY_SIZE = 10000000;

class Quicksort {

public:
    IntArray *processArray = nullptr;

    ProcessGroup *globalGroup;
    ProcessGroup *currentGroup;

    int pivot = 0;

    explicit Quicksort(string arrayFilename) {
        globalGroup = new ProcessGroup(MPI_COMM_WORLD);
        currentGroup = globalGroup;
        if (globalGroup->processId == MAIN_PROCESS) {
            readInputs(arrayFilename);
        }
    }

    void sort() {
        sendParts(globalGroup);
        processArray = recvArray(currentGroup, MAIN_PROCESS);
        while(true) {

            if (currentGroup->processesNum == 1) {
                quickSort(processArray->values, 0, processArray->size - 1);
                break;
            }

            sendPivot(currentGroup);
            quickSort(processArray->values, 0, processArray->size - 1);
            exchangeParts(currentGroup);
            separateProcess(currentGroup);
        }

        if (globalGroup->processId == MAIN_PROCESS) {
            writeArray(join());
        } else {
            join();
        }
    }

    void writeArray(IntArray *array) {
        ofstream stream("quicksort.output");
        stream << array->size << endl;
        for (auto i = 0; i < array->size; i++) {
            stream << array->values[i] << " ";
        }
        stream.close();
    }
private:

    IntArray *join() {
        sendArray(processArray, MAIN_PROCESS, globalGroup->communicator);

        if (globalGroup->processId == MAIN_PROCESS) {
            IntArray *result = nullptr;
            for (int process = 0; process < globalGroup->processesNum; ++process) {
                auto *array = recvArray(globalGroup, process);

                if (result == nullptr) {
                    result = array;
                } else {
                    result = new IntArray(result, array);
                }
            }
            return result;
        }
        return nullptr;
    }

    void sendParts(ProcessGroup *group) {
        if (group->processId == MAIN_PROCESS) {
            for (int process = 0; process < group->processesNum; ++process) {
                auto part = processArray->getPart(process, group->processesNum);
                sendArray(part, process, group->communicator);
            }
        }
    };

    void sendPivot(ProcessGroup *group) {
        if (group->processId == MAIN_PROCESS) {
            pivot = processArray->getPivot();
        }
        MPI_Bcast(&pivot, 1, MPI_INT, MAIN_PROCESS, group->communicator);
    }

    void exchangeParts(ProcessGroup *group) {
        IntArray *leftPart, *rightPart;
        int neighbour = group->getNeighbour();

        processArray->separate(pivot, &leftPart, &rightPart);

        bool isRight = group->isRight();

        if (isRight) {
            sendArray(leftPart, neighbour, group->communicator);
        } else {
            sendArray(rightPart, neighbour, group->communicator);
        }

        auto receivedArray = recvArray(group, neighbour);
        if (isRight) {
            processArray = new IntArray(rightPart, receivedArray);
        } else {
            processArray = new IntArray(leftPart, receivedArray);
        }
    }

    void separateProcess(ProcessGroup *group) {
        int newGroup = group->isRight() ? 1 : 0;
        MPI_Comm communicator;
        MPI_Comm_split(group->communicator, newGroup, 0, &communicator);
        currentGroup = new ProcessGroup(communicator);
    }

    void sendArray(IntArray *part, int destination, MPI_Comm communicator) {
        MPI_Request request;
        MPI_Isend(part->values, part->size, MPI_INT, destination, 0, communicator, &request);
    }

    IntArray *recvArray(ProcessGroup *group, int from) {
        int size;
        MPI_Status status;
        int *buf = new int[MAX_ARRAY_SIZE];
        MPI_Recv(buf, MAX_ARRAY_SIZE, MPI_INT, from, 0, group->communicator, &status);
        MPI_Get_count(&status, MPI_INT, &size);

        return new IntArray(buf, size);
    }

    void readInputs(string filename) {
        ifstream arrayFile(filename);
        int size;
        arrayFile >> size;
        int *values = new int[size];
        for (int i = 0; i < size; ++i) {
            arrayFile >> values[i];
        }
        arrayFile.close();

        processArray = new IntArray(values, size);
    }

    static int partition (int arr[], int low, int high)
    {
        int pivot = arr[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    void quickSort(int arr[], int low, int high)
    {
        if (low < high)
        {
            int pi = partition(arr, low, high);

            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }
};
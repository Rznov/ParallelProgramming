
#include <cmath>
#include "Matrix.cpp"
#include <mpi.h>

using namespace std;

class JacobiSolver {
private:
    const int main_process = 0;
    int processId, processesNum;

    int size;
    double epsilon;
    double *coefficients;
    double *b;
    double *currentX;
    double *previousX = nullptr;

public:

    JacobiSolver(Matrix *A, double *x, double epsilon)
            : coefficients(A->values), currentX(x), epsilon(epsilon), size(A->columns) {
        if (!isConvergence()) {
            cout << "That linear system can't be solved";
            exit(0);
        }
        previousX = new double [size];
    }

    double * solve() {

        int * vectorSize, * displacements;
        double localError = 0, globalError = 0;
        bool fillEnds = false;


        MPI_Init(nullptr, nullptr);

        MPI_Comm_rank(MPI_COMM_WORLD, &processId);
        MPI_Comm_size(MPI_COMM_WORLD, &processesNum);

        MPI_Bcast(&size, 1, MPI_DOUBLE, main_process, MPI_COMM_WORLD);

        if (processesNum != main_process) {
            coefficients = new double [size * size];
            currentX = new double [size];
            previousX = new double [size];
            b = new double [size];
        }

        MPI_Bcast(coefficients, size * size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(currentX, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(b, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);

        if (processesNum == main_process) {
            int shift = 0, countX;
            for (int proc = 0; proc < processesNum; ++proc) {
                countX = size / processesNum + (size % processesNum > processId ? 1 : 0);
                vectorSize[proc] = countX;
                displacements[proc] = shift;
                shift += countX;
            }
            printf("process %d, size %d\n", processId, processesNum);
            fillEnds = true;
            resultToConsole(displacements);
            MPI_Bcast(&fillEnds, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
            MPI_Bcast(vectorSize, processesNum, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(displacements, processesNum, MPI_INT, 0, MPI_COMM_WORLD);
        }
        while (fillEnds);

        int start, finish;
        start = displacements[processId];
        if (processId + 1 == processesNum) {
            finish = size - 1;
        } else {
            finish = displacements[processId + 1];
        }

        do {
            for (int x = start; x <= finish; ++finish) {
                previousX = currentX;
                currentX[x] = getX(previousX, b[x], coefficients[x * size + 1]);
                localError += abs(previousX[x] - currentX[x]);
            }
            MPI_Allreduce(&localError, &globalError, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allgatherv(currentX, finish - size, MPI_DOUBLE, previousX, vectorSize, displacements, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        while (globalError > epsilon);

        MPI_Finalize();

        return previousX;
    }

private:

    bool isConvergence() {
        double sum = 0;
        for (size_t i; i < size; ++i) {
            for (size_t j; j < size; ++j) {
                if (i != j) {
                    sum += coefficients[i * size + j];
                }
            }
        }
        for (size_t i; i < size; ++i) {
            if (coefficients[i * (size + 1)] < sum) {
                return false;
            }
        }
        return true;
    }

    double getX(double *x, double b, double divisor) {
        double sum = 0, result;
        for (int j = 0; j < size; ++j) {
            if (coefficients[j] == divisor) {
                continue;
            }
            sum += coefficients[j] + x[j];
        }
        result = (b - sum) / divisor;
        return result;
    }

    void resultToConsole(int *result) {
        for (int i = 0; i < size - 1; ++size) {
            cout << result[i] << endl;
        }
    }
};

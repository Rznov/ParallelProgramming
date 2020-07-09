
#include <cmath>
#include "Matrix.cpp"
#include <mpi.h>

using namespace std;

class JacobiSolver {
public:
    const int main_process = 0;
    int processId, processesNum;

private:
    int size;
    double *coefficients;
    double *b;
    double *currentX;
    double *previousX = nullptr;

public:

    JacobiSolver() {
        MPI_Comm_rank(MPI_COMM_WORLD, &processId);
        MPI_Comm_size(MPI_COMM_WORLD, &processesNum);
    }

    ~JacobiSolver() {
        delete[] coefficients;
        delete[] b;
        delete[] currentX;
        delete[] previousX;
    }

    double *solve(string coefficientsFilename, string estimateFilename, double error) {

        int *vectorSize, *displacements;
        double localError = 0, globalError = 0;

        vectorSize = new int[processesNum];
        displacements = new int[processesNum];

        if (processId == main_process) {
            readInputs(coefficientsFilename, estimateFilename);
            if (!isConvergence()) {
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }

        MPI_Bcast(&size, 1, MPI_INT, main_process, MPI_COMM_WORLD);
        previousX = new double [size];

        if (processId == main_process) {
            int shift = 0, countX;
            for (int proc = 0; proc < processesNum; ++proc) {
                countX = size / processesNum + (size % processesNum > proc);
                vectorSize[proc] = countX;
                displacements[proc] = shift;
                if (proc != main_process) {
                    MPI_Send(&coefficients[shift * size], countX * size, MPI_DOUBLE, proc, 0, MPI_COMM_WORLD);
                    MPI_Send(&b[shift], countX, MPI_DOUBLE, proc, 0, MPI_COMM_WORLD);
                }
                shift += countX;
            }
        }

        MPI_Bcast(vectorSize, processesNum, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(displacements, processesNum, MPI_INT, 0, MPI_COMM_WORLD);

        if (processId != main_process) {
            currentX = new double [size];

            MPI_Status status;
            int localSize = vectorSize[processId];
            coefficients = new double[size * localSize];
            MPI_Recv(coefficients, size * localSize, MPI_DOUBLE, main_process, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            b = new double[localSize];
            MPI_Recv(b, localSize, MPI_DOUBLE, main_process, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
        saveX();

        int start, finish;
        start = displacements[processId];
        if (processId + 1 == processesNum) {
            finish = size - 1;
        } else {
            finish = displacements[processId + 1] - 1;
        }

        do {
            localError = 0;
            if (processId == main_process) {
                globalError = 0;
            }
            for (int x = start; x <= finish; ++x) {
                computeX(x);
                localError += abs(currentX[x] - previousX[x]);
            }
            MPI_Allreduce(&localError, &globalError, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allgatherv(&currentX[start], vectorSize[processId], MPI_DOUBLE, previousX, vectorSize, displacements, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        while (globalError > error);

        return previousX;
    }

private:

    void readInputs(string coefficientsFilename, string estimateFilename) {
        ifstream coefFile(coefficientsFilename);
        int rows, columns;
        coefFile >> rows >> columns;
        size = rows;
        b = new double[size];
        coefficients = new double[rows * columns];
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                if (j == columns - 1) {
                    coefFile >> b[i];
                    continue;
                }
                coefFile >> coefficients[i * (columns - 1) + j];
            }
        }
        coefFile.close();

        ifstream estimFile(estimateFilename);
        estimFile >> rows >> columns;
        currentX = new double [rows];
        for (size_t i = 0; i < rows; ++i) {
            estimFile >> currentX[i];
        }
        estimFile.close();
    }

    void saveX() {
        if (previousX == nullptr) {
            previousX = new double[size];
        }
        for (int i = 0; i < size; i++) {
            previousX[i] = currentX[i];
        }
    }

    bool isConvergence() {
        double sum = 0;
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                if (i != j) {
                    sum += abs(coefficients[i * size + j]);
                }
            }
            if (sum > coefficients[i * size + i]) {
                return false;
            }
            sum = 0;
        }
        return true;
    }

    void computeX(size_t row) {
        double sum = 0;
        double divisor = coefficients[row * size + row];

        for (size_t j = 0; j < size; ++j) {
            double coefficient = coefficients[row * size + j];
            if (coefficient == divisor) {
                continue;
            }
            sum += coefficient * previousX[j];
        }
        sum = b[row] - sum;
        currentX[row] = sum / divisor;
    }

    void log(const string &message) {
        ofstream stream("ls." + to_string(processId) + "-" + to_string(processesNum) + ".log", ios_base::app);
        stream << time(nullptr) << " - " << message << endl;
        stream.close();
    }

    static string toString(double *array, int size) {
        string contentString;
        if (size < 20) {
            for (int i = 0; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        } else {
            for (int i = 0; i < 5; i++) {
                contentString += to_string(array[i]) + " ";
            }
            contentString += " ... ";
            for (int i = size - 5; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        }
        return "|" + to_string(size) + "|[" + contentString + "]";
    }
};

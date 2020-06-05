
#include <cmath>
#include "Matrix.cpp"
#include <mpi.h>

using namespace std;

class JacobiSolver {
private:
    const int main_process = 0;
    int processId, processesNum;

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

    double *solve(string coefficientsFilename, string estimateFilename, double error) {

        int *vectorSize, *displacements;
        double localError = 0, globalError = 0;
        bool fillEnds = false;

        //printf("Hello world from processor rank %d out of %d processors\n", processId, processesNum);

        if (processId == main_process) {
            //works till here
            readInputs(coefficientsFilename, estimateFilename);
            //log(toString(b, size));
            if (!isConvergence()) {
                cout << "That linear system can't be solved";
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }

        MPI_Bcast(&size, 1, MPI_DOUBLE, main_process, MPI_COMM_WORLD);

        if (processId != main_process) {
            coefficients = new double [size * size];
            currentX = new double [size];
            previousX = new double [size];
            b = new double [size];
        }

        MPI_Bcast(coefficients, size * size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(currentX, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(b, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);

        if (processId == main_process) {
            int shift = 0, countX;
            for (int proc = 0; proc < processesNum; ++proc) {
                countX = size / processesNum + (size % processesNum > processId ? 1 : 0);
                vectorSize[proc] = countX;
                displacements[proc] = shift;
                shift += countX;
            }
            //printf("process %d, size %d\n", processId, processesNum);
            fillEnds = true;
            //resultToConsole(displacements);
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
        while (globalError > error);

        return previousX;
    }

private:

    void readInputs(string coefficientsFilename, string estimateFilename) {
        ifstream coefFile(coefficientsFilename);
        int rows, columns;
        coefFile >> rows >> columns;
//        string rowcol = to_string(rows) + " " + to_string(columns);
//        log(rowcol);
        size = rows;
        b = new double[size];
        coefficients = new double[rows * columns];
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                if (j == columns - 1) {
                    coefFile >> b[rows];
                    string bs = to_string(b[rows]);
                    log(bs);
                    continue;
                }
                coefFile >> coefficients[i * rows + j];
                string cs = to_string(coefficients[i * rows + j]);
                log(cs);
            }
        }
        coefFile.close();
        string bs = to_string(b[1]);
        log(bs);

        ifstream estimFile(estimateFilename);
        estimFile >> rows >> columns;
        currentX = new double [rows];
        for (size_t i = 0; i < rows; ++i) {
            estimFile >> currentX[rows];
        }
        estimFile.close();
    }

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

    void resultToConsole(double *result) {
        for (int i = 0; i < size - 1; ++size) {
            cout << result[i] << endl;
        }
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

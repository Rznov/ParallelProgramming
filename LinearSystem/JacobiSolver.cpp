
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

        vectorSize = new int[processesNum];
        displacements = new int[processesNum];

        if (processId == main_process) {
            readInputs(coefficientsFilename, estimateFilename);
            if (!isConvergence()) {
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }

        MPI_Bcast(&size, 1, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        previousX = new double [size];

        if (processId != main_process) {
            coefficients = new double [size * size];
            currentX = new double [size];
            b = new double [size];
        }

        MPI_Bcast(coefficients, size * size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(currentX, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);
        MPI_Bcast(b, size, MPI_DOUBLE, main_process, MPI_COMM_WORLD);

        if (processId == main_process) {
            int shift = 0, countX;
            for (int proc = 0; proc < processesNum; ++proc) {
                countX = size / processesNum + (size % processesNum > proc ? 1 : 0);
                vectorSize[proc] = countX;
                displacements[proc] = shift;
                shift += countX;
            }
        }
        MPI_Bcast(vectorSize, processesNum, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(displacements, processesNum, MPI_INT, 0, MPI_COMM_WORLD);

        int start, finish;
        start = displacements[processId];
        if (processId + 1 == processesNum) {
            finish = size - 1;
        } else {
            finish = displacements[processId + 1] - 1;
        }
        //works till here

//        do {
            for (int x = start; x <= finish; ++finish) {
                //log(to_string(x));
                saveCurrentXAsPrevious();
                //log(toString(previousX, size));
                computeX(x);
//                log(to_string(currentX[x]));
//                log(to_string(previousX[x]));
                break;
                localError += abs(previousX[x] - currentX[x]);
            }
//            MPI_Allreduce(&localError, &globalError, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
//            MPI_Allgatherv(currentX, finish - size, MPI_DOUBLE, previousX, vectorSize, displacements, MPI_DOUBLE, MPI_COMM_WORLD);
        //}
//        while (globalError > error);

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
                    b[i] *= -1;
                    continue;
                }
                coefFile >> coefficients[i * rows + j];
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

    void saveCurrentXAsPrevious() {
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
                    sum += coefficients[i * size + j];
                }
            }
        }
        for (size_t i = 0; i < size; ++i) {
            if (coefficients[i * (size + 1)] < sum) {
                return false;
            }
        }
        return true;
    }

    void computeX(size_t row) {
        double sum = 0;
        double divisor = coefficients[row * size + row];

        for (size_t j = 0; j < size; ++size) {
            double coef = coefficients[row * size + j];

            if (coef == divisor) {
                continue;
            }
            sum += coef * previousX[j];
        }

        currentX[row] = sum / divisor;
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

    static string toString(int *array, int size) {
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

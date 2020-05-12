#include "Matrix.cpp"

class Multiplication {
public:
    Matrix *a;
    Matrix *b;
    time_t start_time = 0;
    time_t finish_time = 0;

    Multiplication(Matrix *a, Matrix *b) : a(a), b(b) {}

    virtual Matrix * multiply() {
        time(&start_time);
        auto result = new Matrix(a->rows, b->columns);
        for (int i = 0; i < result->rows; ++i) {
            for (int j = 0; j < result->columns; ++j) {
                result->values[i][j] = getElement(i, j);
            }
        }
        time(&finish_time);
        return result;
    }

protected:
    int getElement(int row, int column) const {
        int element = 0;
        for (int i = 0; i < a->columns; ++i) {
            element += a->values[row][i] * b->values[i][column];
        }
        return element;
    }
};

class ParallelForStaticScheduleMultiplication: public Multiplication {
public:
    ParallelForStaticScheduleMultiplication(Matrix * a, Matrix * b) : Multiplication(a, b) {}

    Matrix * multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto result = new Matrix(a->rows, b->columns);

        time(&start_time);
#pragma omp parallel for schedule(static)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        time(&finish_time);
        return result;
    }
};

class ParallelForDynamicScheduleMultiplication: public Multiplication {
public:
    ParallelForDynamicScheduleMultiplication(Matrix * a, Matrix * b) : Multiplication(a, b) {}

    Matrix * multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto result = new Matrix(a->rows, b->columns);

        time(&start_time);
#pragma omp parallel for schedule(dynamic)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        time(&finish_time);
        return result;
    }
};

class ParallelForGuidedScheduleMultiplication: public Multiplication {
public:
    ParallelForGuidedScheduleMultiplication(Matrix * a, Matrix * b) : Multiplication(a, b) {}

    Matrix * multiply() override {
        int rows = a->rows;
        int columns = b->columns;
        int tasks = rows * columns;
        auto * result = new Matrix(a->rows, b->columns);

        time(&start_time);
#pragma omp parallel for schedule(guided)
        for (int task = 0; task < tasks; ++task) {
            int row = task / columns;
            int column = task % columns;
            result->values[row][column] = getElement(row, column);
        }
        time(&finish_time);
        return result;
    }
};
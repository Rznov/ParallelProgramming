#include <iostream>
#include "Matrix.cpp"
#include "Multiplication.cpp"

using namespace std;

int main() {

    int rows = 0;
    int columns = 0;
    cout<<"Enter matrix size:"<<endl<<"rows:";
    cin>>rows;
    cout<<"columns:";
    cin>>columns;
    auto A = new Matrix(rows, columns);
    auto B = new Matrix(rows,columns);
    A->fillRandom();
    B->fillRandom();

    auto serialMultiplication = new Multiplication(A, B);
    auto staticMultiplication = new ParallelForStaticScheduleMultiplication(A, B);
    auto dynamicMultiplication = new ParallelForDynamicScheduleMultiplication(A, B);
    auto guidedMultiplication = new ParallelForGuidedScheduleMultiplication(A, B);

    serialMultiplication->multiply()->writeToFile();
    staticMultiplication->multiply()->writeToFile();
    dynamicMultiplication->multiply()->writeToFile();
    guidedMultiplication->multiply()->writeToFile();

//    A->writeToConsole();
//    B->writeToConsole();

    cout<<"Serial multiplication time:"<<difftime(serialMultiplication->finish_time, serialMultiplication->start_time)<<endl;
//    cout<<"Parallel multiplication with static schedule time:"<<difftime(staticMultiplication->finish_time, staticMultiplication->start_time)<<endl;
//    cout<<"Parallel multiplication with dynamic schedule time:"<<difftime(dynamicMultiplication->finish_time, dynamicMultiplication->start_time)<<endl;
//    cout<<"Parallel multiplication with guided schedule time:"<<difftime(guidedMultiplication->finish_time, guidedMultiplication->start_time)<<endl;

    return 0;
}

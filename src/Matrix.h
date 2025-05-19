#include <stdlib.h>
#include <fstream>
using namespace std;

class Matrix {
public:
    int n;
    int **mat;

    Matrix();
    Matrix(int n_);
    ~Matrix();

    void alloc(int n);
    void read(ifstream &fin);
    void print(ofstream &fout);

    static Matrix &multiply(const Matrix &a, const Matrix &b);
    static Matrix &multiplyParallel1(const Matrix &a, const Matrix &b);
    static Matrix &multiplyParallel2(const Matrix &a, const Matrix &b);
};

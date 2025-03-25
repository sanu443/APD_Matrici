#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

#define NR_INPUTS 10 
#define REPEAT 1

using namespace std;

class Matrix {
public:
    int n;
    int **mat;

    void alloc(int n) {
        mat = new int*[n];
        for (int i = 0; i < n; i++)
            mat[i] = new int[n];
    }

    Matrix(int n_) {
        n = n_;
        alloc(n);
    }

    Matrix() {}

    ~Matrix() {
        for (int i = 0; i < n; i++)
            free(mat[i]);
        free(mat);
    }

    void read(ifstream &fin) {
        fin >> n;
        alloc(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                fin >> mat[i][j];
    }

    void print(ofstream &fout) {
        fout << "\n";
        for (int i = 0; i < n; i++, fout<<"\n")
            for (int j = 0; j < n; j++)
                fout << mat[i][j] <<" ";
    }

    static Matrix &multiply (const Matrix &a, const Matrix &b) {
        int n = a.n;
        Matrix &c = *new Matrix(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                c.mat[i][j] = 0;
                for (int k = 0; k < n; k++)
                    c.mat[i][j] += a.mat[i][k] * b.mat[k][j];
            }
        return c;
    }
};

int main () {
    ifstream fin;
    ofstream fout;
    string results_file ="Results//results.txt";
    fout.open(results_file);
    for (int test = 0; test < NR_INPUTS; test++) {
        string in_file = "Inputs//in" + to_string(test) + ".txt";
        string out_file = "Outputs//out" + to_string(test) + ".txt";
        Matrix &a = *new Matrix(), &b = *new Matrix();
        fin.open(in_file);
        a.read(fin);
        b.read(fin);
        fin.close();
        long long sum = 0;
        for (int times = 0; times < REPEAT; times++) {
            auto start = chrono::high_resolution_clock::now();
            Matrix &c = Matrix::multiply(a, b);
            auto end = chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            sum += duration.count();
            delete &c;
        }
        int mean =  sum / REPEAT;
        fout << "Testul #" << test << " durata: " << mean << " ms.\n";
        //Matrix &c =Matrix::multiply(a, b);
        //c.print(fout);
        delete &a;
        delete &b;
        //delete &c;
    }
    fout.close();
}

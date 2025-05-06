#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;

int main() {
    string results_file = "..//Results//resultsSecvential.txt";
    ofstream results(results_file);

    for (int test = 1; test <= 5; ++test) {
        string in_file  = "..//Inputs//in"  + to_string(test) + ".txt";
        string out_file = "..//Outputs//out" + to_string(test) + ".txt";

        ifstream fin(in_file);
        ofstream fout(out_file);

        Matrix A, B;
        A.read(fin);
        B.read(fin);

        auto start = chrono::high_resolution_clock::now();
        Matrix C = Matrix::multiply(A, B);
        auto end   = chrono::high_resolution_clock::now();

        long long duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        results << duration << "\n";

        C.print(fout);

        fin.close();
        fout.close();
    }

    results.close();
    return 0;
}

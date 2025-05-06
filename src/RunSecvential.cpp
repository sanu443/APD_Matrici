#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include "Matrix.h"

#define NR_INPUTS 10
#define REPEAT 5

using namespace std;

int main() {
    ifstream fin;
    ofstream fout;
    string results_file = "..//Results//resultsSecvential.txt";
    ofstream execution_time_out(results_file);

    for (int test = 0; test < NR_INPUTS; test++) {
        Matrix &a = *new Matrix(), &b = *new Matrix();
        string in_file = "..//Inputs//in" + to_string(test) + ".txt";
        fin.open(in_file);
        a.read(fin);
        b.read(fin);
        fin.close();

        long long sum = 0;
        for (int times = 0; times < REPEAT; times++) {
            auto start = chrono::high_resolution_clock::now();
            Matrix &c = Matrix::multiply(a, b);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            sum += duration.count();
            delete &c;
        }

        double mean = (sum * 1.0) / (REPEAT * 1.0);
        string convert_unit_str = "";
        if (mean > 60 * 1000) {
            int minutes = ((int)mean) / (60 * 1000);
            int seconds = (mean - 60 * 1000 * minutes) / 1000;
            convert_unit_str = "(" + to_string(minutes) + " m " + to_string(seconds) + " s)";
        } else if (mean > 1000) {
            convert_unit_str = "(" + to_string(mean / 1000.0) + " s)";
        }

        execution_time_out << "Testul #" << test
                           << " durata: " << mean << " ms"
                           << convert_unit_str << ".\n";
        
        Matrix &c = Matrix::multiplyParallel1(a, b);
        string out_file = "..//OutputsSecvential//out" + to_string(test) + ".txt";
        fout.open(out_file);
        c.print(fout);
        fout.close();

        delete &a;
        delete &b;
        delete &c;
    }

    execution_time_out.close();
    return 0;
}

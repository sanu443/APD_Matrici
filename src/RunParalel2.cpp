#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <mpi.h>
#include "Matrix.h"

#define NR_INPUTS 5
#define REPEAT 5

using namespace std;

int main(int argc, char** argv) {   
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ifstream fin;
    ofstream fout;
    string results_file = "..//Results//resultsParalel2.txt";
    ofstream execution_time_out;
    if (rank == 0) execution_time_out.open(results_file);

    for (int test = 0; test < NR_INPUTS; test++) {
        Matrix *a = new Matrix(), *b = new Matrix();
        string in_file = "..//Inputs//in" + to_string(test) + ".txt";
        fin.open(in_file);
        a->read(fin);
        b->read(fin);
        fin.close();

        long long sum = 0;
        for (int times = 0; times < REPEAT; times++) {
            MPI_Barrier(MPI_COMM_WORLD);    
            auto start = chrono::high_resolution_clock::now();
            Matrix &c = Matrix::multiplyParallel2(*a, *b);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            if (rank == 0) {
                sum += duration.count();
                delete &c;
            }
        }

        if (rank == 0) {
            double mean = (sum * 1.0) / REPEAT;
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

            Matrix &c = Matrix::multiplyParallel2(*a, *b);
            string out_file = "..//OutputsParalel2//out" + to_string(test) + ".txt";
            fout.open(out_file);
            c.print(fout);
            fout.close();
            delete &c;
        }

        delete a;
        delete b;
    }

    if (rank == 0)
        execution_time_out.close();

    MPI_Finalize();
    return 0;
}
    
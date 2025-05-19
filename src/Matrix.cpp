#include "Matrix.h"
#include <mpi.h>    
#include <stdlib.h>
#include <fstream>
#include <thread>

using namespace std;
    
void Matrix::alloc(int n) {
    mat = new int*[n];
    for (int i = 0; i < n; i++)
        mat[i] = new int[n];
}

Matrix::Matrix(int n_) {
    n = n_;
    alloc(n);
}

Matrix::Matrix() {}

Matrix::~Matrix() {
    for (int i = 0; i < n; i++)
        free(mat[i]);
    free(mat);
}

void Matrix::read(ifstream &fin) {
    fin >> n;
    alloc(n);
    for (int i = 0; i < n; i++) 
        for (int j = 0; j < n; j++)
            fin >> mat[i][j];
}

void Matrix::print(ofstream &fout) {
    fout << "\n";
    for (int i = 0; i < n; i++, fout<<"\n")
        for (int j = 0; j < n; j++)
            fout << mat[i][j] <<" ";
}

static void multiplyWorker(const Matrix *a, const Matrix *b, Matrix *c, int start_row, int end_row) {
    int n = a->n;
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < n; ++j) {
            int sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += a->mat[i][k] * b->mat[k][j];
            }
            c->mat[i][j] = sum;
        }
    }
}

Matrix& Matrix::multiplyParallel1(const Matrix &a, const Matrix &b) {
    int n = a.n;
    Matrix &c = *new Matrix(n);

    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) 
        num_threads = 12; //am 12 core-uri logice

    std::thread *threads = new std::thread[num_threads];

    int chunk = n / num_threads;
    int rem   = n % num_threads;
    int start = 0;

    for (int t = 0; t < num_threads; ++t) {
        int end = start + chunk + (t < rem ? 1 : 0);
        threads[t] = std::thread(multiplyWorker, &a, &b, &c, start, end);
        start = end;
    }   

    for (int t = 0; t < num_threads; ++t)
        threads[t].join();

    delete[] threads;
    return c;
}




Matrix& Matrix::multiply (const Matrix &a, const Matrix &b) {
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

Matrix& Matrix::multiplyParallel2(const Matrix& a, const Matrix& b) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = a.n;
    Matrix* c = new Matrix(n);


    for (int i = 0; i < n; ++i)
        MPI_Bcast(b.mat[i], n, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk = n / size;
    int rem = n % size;
    int start = rank * chunk + std::min(rank, rem);
    int end = start + chunk + (rank < rem ? 1 : 0);
    Matrix localA(end - start);
    for (int i = 0; i < end - start; ++i)
        localA.mat[i] = new int[n];

    if (rank == 0) {
        for (int proc = 0; proc < size; ++proc) {
            int s = proc * chunk + std::min(proc, rem);
            int e = s + chunk + (proc < rem ? 1 : 0);
            for (int i = s; i < e; ++i) {
                if (proc == 0)
                    memcpy(localA.mat[i - s], a.mat[i], n * sizeof(int));
                else
                    MPI_Send(a.mat[i], n, MPI_INT, proc, i, MPI_COMM_WORLD);
            }
        }
    } else {
        for (int i = 0; i < end - start; ++i)
            MPI_Recv(localA.mat[i], n, MPI_INT, 0, start + i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    Matrix localC(end - start);
    for (int i = 0; i < end - start; ++i) {
        for (int j = 0; j < n; ++j) {
            localC.mat[i][j] = 0;
            for (int k = 0; k < n; ++k)
                localC.mat[i][j] += localA.mat[i][k] * b.mat[k][j];
        }
    }

    // Rezultatul se trimite la procesul 0
    if (rank == 0) {
        for (int i = 0; i < end - start; ++i)
            memcpy(c->mat[start + i], localC.mat[i], n * sizeof(int));

        for (int proc = 1; proc < size; ++proc) {
            int s = proc * chunk + std::min(proc, rem);
            int e = s + chunk + (proc < rem ? 1 : 0);
            for (int i = s; i < e; ++i)
                MPI_Recv(c->mat[i], n, MPI_INT, proc, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        for (int i = 0; i < end - start; ++i)
            MPI_Send(localC.mat[i], n, MPI_INT, 0, start + i, MPI_COMM_WORLD);
    }

    return *c;
}



#include "Matrix.h"
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

            unsigned int num_threads = std::thread::hardware_concurrency();
            if (num_threads == 0) num_threads = 6;

            std::thread *threads = new std::thread[num_threads];

            int chunk = n / num_threads;
            int rem   = n % num_threads;
            int start = 0;

            for (int t = 0; t < num_threads; ++t) {
                int end = start + chunk + (t < rem ? 1 : 0);
                threads[t] = std::thread(multiplyWorker, &a, &b, &c, start, end);
                start = end;
            }

            for (int t = 0; t < num_threads; ++t) {
                threads[t].join();
            }

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

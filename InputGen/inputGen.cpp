#include <fstream>
#include <string>
#include <random>
#include <time.h>

#define GENERATOR 10
#define MAX_VALUE 100

using namespace std;

int main() {
    int n = 4;
    srand(time(NULL));
    for (int test = 0; test < GENERATOR; test++) {
        string out_file = "../Inputs/in" + to_string(test) + ".txt";
        ofstream fout(out_file);        
        n *= 2;
        for (int times = 0; times < 2; times++) {
            fout << "\n" << n << "\n";
            for (int i = 0; i < n; i++, fout << "\n")
                for (int j = 0; j < n; j++)
                    fout << rand() % MAX_VALUE + 1 << " ";
        }
        /*
        fout << "\n" << n << "\n";
        for (int i = 0; i < n; i++, fout << "\n")
            for (int j = 0; j < n; j++)
                fout << ((i == j) ? 1 : 0) << " ";
        */
    }
}
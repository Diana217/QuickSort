#include <iostream>
#include <vector>
#include <mpi.h>
#include <fstream> 
#include <algorithm> 
#include <chrono> 
#include <sstream> 

void quick_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    quick_sort(arr, left, j);
    quick_sort(arr, i, right);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::ifstream infile("C:\\Users\\dulko\\source\\repos\\FileGeneration\\input10.txt");
    if (!infile.is_open()) {
        std::cerr << "Failed to open input file." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    std::vector<int> arr;
    auto start = std::chrono::steady_clock::now();

    int n = 0;
    if (rank == 0) {
        int num;
        while (infile >> num) {
            arr.push_back(num);
        }
        infile.close();
        n = arr.size();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        arr.resize(n);
    }

    MPI_Bcast(arr.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    quick_sort(arr, 0, arr.size() - 1);

    if (rank == 0) {
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << "Sorting time: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
#include <iostream>
#include <vector>
#include <mpi.h>
#include <fstream> 
#include <algorithm> 
#include <chrono> 
#include <sstream> 

void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::ifstream infile("C:\\Users\\dulko\\source\\repos\\FileGeneration\\input3.txt");
    if (!infile.is_open()) {
        std::cerr << "Failed to open input file." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    const int num_threads[] = { 1, 2, 4, 8 };

    for (int i = 0; i < sizeof(num_threads) / sizeof(num_threads[0]); ++i) {
        int threads = num_threads[i];

        std::vector<int> arr;
        auto start = std::chrono::steady_clock::now();

        if (rank == 0) {
            int num;
            while (infile >> num) {
                arr.push_back(num);
            }
            infile.close();

            int n = arr.size();
            MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(arr.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else {
            int n;
            MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
            arr.resize(n);
            MPI_Bcast(arr.data(), n, MPI_INT, 0, MPI_COMM_WORLD);
        }

        merge_sort(arr, 0, arr.size() - 1);

        if (rank == 0) {
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            std::cout << "Sorting time with " << threads << " threads: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}

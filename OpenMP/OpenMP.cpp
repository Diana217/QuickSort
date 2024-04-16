#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <chrono>

void quick_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int i = left, j = right;
    int pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            quick_sort(arr, left, j);
        }
#pragma omp section
        {
            quick_sort(arr, i, right);
        }
    }
}

int main() {
    std::ifstream infile("C:\\Users\\dulko\\source\\repos\\FileGeneration\\input3.txt");
    if (!infile.is_open()) {
        std::cerr << "Failed to open input file." << std::endl;
        return 1;
    }

    std::vector<int> arr;
    int num;
    while (infile >> num) {
        arr.push_back(num);
    }
    infile.close();

    int n = arr.size();

    const int num_threads[] = { 1, 2, 4, 8 };
    for (int i = 0; i < sizeof(num_threads) / sizeof(num_threads[0]); ++i) {
        int threads = num_threads[i];
        omp_set_num_threads(threads);

        auto start = std::chrono::steady_clock::now();

#pragma omp parallel
        {
#pragma omp single
            {
                quick_sort(arr, 0, n - 1);
            }
        }

        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;

        /*std::stringstream ss;
        ss << threads;
        std::string thread_str = ss.str();*/

        std::cout << "Sorting time with " << threads << " thread(s): " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

        /*std::ofstream outfile("sorted_output_" + thread_str + "_threads.txt");
        if (!outfile.is_open()) {
            std::cerr << "Failed to open output file." << std::endl;
            return 1;
        }
        for (int num : arr) {
            outfile << num << " ";
        }
        outfile.close();

        std::cout << "Sorted numbers with " << threads << " thread(s) written to file" << std::endl;*/
    }

    return 0;
}
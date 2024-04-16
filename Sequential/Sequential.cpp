#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

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

    auto start = std::chrono::steady_clock::now();

    quick_sort(arr, 0, n - 1);

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << "Sorting time: " << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

    std::ofstream outfile("sorted_output1.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }
    for (int num : arr) {
        outfile << num << " ";
    }
    outfile.close();

    std::cout << "Sorted numbers written to file" << std::endl;

    return 0;
}
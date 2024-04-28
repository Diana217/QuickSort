#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>

// ��������� ��������� �������� ����������
void quickSort(std::vector<int>& values, int left, int right) {
    if (left >= right) return; // ����� ������� ������
    int i = left, j = right;
    int pivot = values[(left + right) / 2]; // ���� �������� ��������
    while (i <= j) {
        while (values[i] < pivot) i++; // ����� ��������, ������� �� �������, ����
        while (values[j] > pivot) j--; // ����� ��������, ������� �� �������, ������
        if (i <= j) {
            std::swap(values[i], values[j]); // ���� ��������
            i++;
            j--;
        }
    }
    // ����������� ������ ���������� ��� �������� ������
    if (left < j) quickSort(values, left, j);
    if (i < right) quickSort(values, i, right);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // ����������� MPI

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // ��������� ����� ��������� �������
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // ��������� �������� ������� �������

    std::vector<int> numbers;
    if (world_rank == 0) {
        // ������� ����� � ����� � ������ � ������ 0
        std::ifstream input_file("C:\\Users\\dulko\\source\\repos\\FileGeneration\\input20.txt");
        int number;
        while (input_file >> number) {
            numbers.push_back(number);
        }
        input_file.close();
    }

    // �������� ������ ������� ��� ��������
    int num_elements = numbers.size();
    MPI_Bcast(&num_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // ���������� ������� ��������, �� ����������� ����� ������
    int n_per_proc = (num_elements + world_size - 1) / world_size; // г�������� ������� ��������

    std::vector<int> local_numbers(n_per_proc);
    std::vector<int> counts(world_size, n_per_proc);
    std::vector<int> displacements(world_size);

    // ���������� ����� �� ������� ����� ��� ������� �������
    for (int i = 0; i < world_size; i++) {
        displacements[i] = i * n_per_proc;
        if (displacements[i] + n_per_proc > num_elements) {
            counts[i] = num_elements - displacements[i];
        }
    }

    // �������� ������ ������� ����� ������� �������
    MPI_Scatterv(numbers.data(), counts.data(), displacements.data(), MPI_INT,
        local_numbers.data(), counts[world_rank], MPI_INT, 0, MPI_COMM_WORLD);

    local_numbers.resize(counts[world_rank]); // ���� ������ ������� �� �������� ����������

    // ������������ ����� ������ ����
    MPI_Barrier(MPI_COMM_WORLD);
    auto start = std::chrono::high_resolution_clock::now();

    // ���������� ��������� �����
    quickSort(local_numbers, 0, local_numbers.size() - 1);

    // ��� ������������ ������� � ������ � ������ 0
    std::vector<int> sorted_numbers;
    if (world_rank == 0) {
        sorted_numbers.resize(num_elements);
    }

    MPI_Gatherv(local_numbers.data(), local_numbers.size(), MPI_INT,
        sorted_numbers.data(), counts.data(), displacements.data(), MPI_INT, 0, MPI_COMM_WORLD);

    auto finish = std::chrono::high_resolution_clock::now(); // ʳ���� ����� ����

    if (world_rank == 0) {
        // ��������� ���� ���������
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        std::cout << "Sorting time: " << elapsed.count() << " ms\n";
    }

    MPI_Finalize(); // ���������� ������ � MPI
    return 0;
}
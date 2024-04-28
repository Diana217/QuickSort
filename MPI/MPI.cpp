#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>

// Реалізація алгоритму швидкого сортування
void quickSort(std::vector<int>& values, int left, int right) {
    if (left >= right) return; // Умова зупинки рекурсії
    int i = left, j = right;
    int pivot = values[(left + right) / 2]; // Вибір опорного елемента
    while (i <= j) {
        while (values[i] < pivot) i++; // Пошук елемента, більшого за опорний, зліва
        while (values[j] > pivot) j--; // Пошук елемента, меншого за опорний, справа
        if (i <= j) {
            std::swap(values[i], values[j]); // Обмін елементів
            i++;
            j--;
        }
    }
    // Рекурсивний виклик сортування для поділених частин
    if (left < j) quickSort(values, left, j);
    if (i < right) quickSort(values, i, right);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv); // Ініціалізація MPI

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Отримання рангу поточного процесу
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Отримання загальної кількості процесів

    std::vector<int> numbers;
    if (world_rank == 0) {
        // Читання чисел з файлу в процесі з рангом 0
        std::ifstream input_file("C:\\Users\\dulko\\source\\repos\\FileGeneration\\input20.txt");
        int number;
        while (input_file >> number) {
            numbers.push_back(number);
        }
        input_file.close();
    }

    // Розсилка розміру вектора всім процесам
    int num_elements = numbers.size();
    MPI_Bcast(&num_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Розрахунок кількості елементів, які оброблятиме кожен процес
    int n_per_proc = (num_elements + world_size - 1) / world_size; // Рівномірний розподіл елементів

    std::vector<int> local_numbers(n_per_proc);
    std::vector<int> counts(world_size, n_per_proc);
    std::vector<int> displacements(world_size);

    // Розрахунок зсувів та кількості чисел для кожного процесу
    for (int i = 0; i < world_size; i++) {
        displacements[i] = i * n_per_proc;
        if (displacements[i] + n_per_proc > num_elements) {
            counts[i] = num_elements - displacements[i];
        }
    }

    // Розсилка частин вектора чисел кожному процесу
    MPI_Scatterv(numbers.data(), counts.data(), displacements.data(), MPI_INT,
        local_numbers.data(), counts[world_rank], MPI_INT, 0, MPI_COMM_WORLD);

    local_numbers.resize(counts[world_rank]); // Зміна розміру вектора до фактично отриманого

    // Синхронізація перед виміром часу
    MPI_Barrier(MPI_COMM_WORLD);
    auto start = std::chrono::high_resolution_clock::now();

    // Сортування отриманих чисел
    quickSort(local_numbers, 0, local_numbers.size() - 1);

    // Збір відсортованих векторів у процесі з рангом 0
    std::vector<int> sorted_numbers;
    if (world_rank == 0) {
        sorted_numbers.resize(num_elements);
    }

    MPI_Gatherv(local_numbers.data(), local_numbers.size(), MPI_INT,
        sorted_numbers.data(), counts.data(), displacements.data(), MPI_INT, 0, MPI_COMM_WORLD);

    auto finish = std::chrono::high_resolution_clock::now(); // Кінець виміру часу

    if (world_rank == 0) {
        // Виведення часу виконання
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        std::cout << "Sorting time: " << elapsed.count() << " ms\n";
    }

    MPI_Finalize(); // Завершення роботи з MPI
    return 0;
}
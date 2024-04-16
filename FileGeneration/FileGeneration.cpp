#include <iostream>
#include <fstream>
#include <random>

int main() {
    const int num_count = 1000000;
    const int min_num = 1; 
    const int max_num = 1000; 

    std::ofstream outfile("input3.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_num, max_num);

    for (int i = 0; i < num_count; ++i) {
        outfile << dis(gen) << " ";
    }

    outfile.close();
    std::cout << "File with random numbers generated successfully." << std::endl;

    return 0;
}

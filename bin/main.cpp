#include <fstream>
#include <iostream>

#include "interpreter.h"

int main(int argc, char** argv) {
    // Хардовый запуск
    // std::string code = R"(
    // print("Hello world!")
    // )";

    // std::istringstream input(code);
    // std::ostringstream output;

    // std::cout << interpret(input, output) << '\n';
    // std::cout << output.str();

    // Пробуем считать из файла, если он подан
    if (argc == 2) {
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << argv[1] << "\n";
            return 1;
        }
        if (!interpret(file, std::cout))
            return 1;
        return 0;
    }
    // Иначе из стандартного потока
    if (!interpret(std::cin, std::cout))
        return 1;
    return 0;
}

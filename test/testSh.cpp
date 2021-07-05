#include <iostream>

#include "spherical_harmonics.hpp"

int main(int argc, char const *argv[]) {
  std::cout << "Y6m:"
            << "\n";
  for (int i = -6; i < 7; i++) {
    std::cout << sh::Y6m(i, sh::dir(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]))) << "\n";
  }
  std::cout << "Y4m:"
            << "\n";
  for (int i = -4; i < 5; i++) {
    std::cout << sh::Y4m(i, sh::dir(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]))) << "\n";
  }

  return 0;
}

// g++ test.cpp libsh.so -O3 -o a

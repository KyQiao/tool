#include <iostream>

#include "Table.hpp"

int main(int argc, char const *argv[]) {
  Table test;
  int col = 10, row = 10;
  test.setcols(col);
  test.setrows(row);

  std::cout << "set row col OK" << std::endl;
  for (int i = 0; i < row; i++)
    for (int j = 0; j < col; j++)
      test.data[i][j] = 1. / (i * i + j * j);
  std::cout << "input OK" << std::endl;
  test.output("file.txt");

  std::cout << test << std::endl;
  return 0;
}

// g++ -fsanitize=address -O3 -Wall -pedantic --std=c++11  testTable.cpp -o test2
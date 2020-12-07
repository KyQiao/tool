#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "Base.hpp"
#include "Steinhardt.hpp"


int main(int argc, char const *argv[]) {
  if ((argc != 2) && (argc != 3)) {
    std::cout << "input number should be one or two" << std::endl;
    std::cout << "qw input.xyz or \n"
              << "qw input.xyz output.qw.xyz"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];

  auto data = Frame();
  if (filename.substr(filename.size() - 2) == "gz")
    data.readLammps(filename, Frame::compressType::gz);
  else
    data.readLammps(filename);
  // q6 q4 w6 w4
  Table result = qw(data);

  if (argc == 2) {
    std::string outputName = filename + ".qw.xyz";
    result.output(outputName);
  } else {
    result.output(std::string(argv[2]));
  }
  return 0;
}

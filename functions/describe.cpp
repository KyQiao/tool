#include <iostream>
#include <string>

#include "Base.hpp"


int main(int argc, char const *argv[]) {
  if ((argc != 2) && (argc != 3)) {
    std::cout << "input number should be one " << std::endl;
    std::cout << "outputXYZ input.xyz " << std::endl;
  }

  std::string filename = argv[1];

  auto data = Frame();

  if(filename.substr(filename.size()-2)=="gz")
    data.readLammps(filename,Frame::compressType::gz);
  else
    data.readLammps(filename);
  data.describe();
  return 0;
}

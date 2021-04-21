#include <fstream>
#include <iostream>
#include <string>

#include "Base.hpp"

void output(const Frame &data, std::string out) {
  std::fstream outfile;
  outfile.open(out, std::ios::out);

  if (data.is2D())
    for (size_t i = 0; i < data.particleN; i++)
      outfile << data.x[i] << " " << data.y[i] << "\n";
  else if (data.is3D())
    for (size_t i = 0; i < data.particleN; i++)
      outfile << data.x[i] << " " << data.y[i] << " " << data.z[i] << "\n";
  outfile.close();

  std::cout << "writing complete" << std::endl;
}

int main(int argc, char const *argv[]) {
  if ((argc != 2) && (argc != 3)) {
    std::cout << "input number should be one or two" << std::endl;
    std::cout << "outputXYZ input.xyz or \n"
              << "outputXYZ input.xyz output.xyz"
              << std::endl;
  }

  std::string filename = argv[1];
  auto data = Frame();
  data.read(filename);

  if (argc == 2) {
    std::string outputName = filename + ".xyzOnly.xyz";
    output(data, outputName);
  } else {
    output(data, std::string(argv[2]));
  }
  return 0;
}

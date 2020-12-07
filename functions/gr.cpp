// calculate pair distribution function with PBC
// return binedge(mid point of left and right edge) and gr

#include "gr.hpp"

#include <iostream>
#include <string>

#include "Base.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 4) && (argc != 5)) {
    std::cout << "input number should be one or two" << std::endl;
    std::cout << "gr input.xyz 100 5 or \n"
              << "gr input.xyz 100 5 output.gr.xyz"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  int bins = std::stoi(argv[2]);
  double maxr = std::stod(argv[3]);

  auto data = Frame();
  if (filename.substr(filename.size() - 2) == "gz")
    data.readLammps(filename, Frame::compressType::gz);
  else
    data.readLammps(filename);

  if (argc == 3) {
    std::string outputName = filename + ".gr.xyz";
    if (data.is2D())
      gr2d(data, bins, maxr).output(outputName);
    else if (data.is3D())
      gr3d(data, bins, maxr).output(outputName);
  } else {
    if (data.is2D())
      gr2d(data, bins, maxr).output(std::string(argv[4]));
    else if (data.is3D())
      gr3d(data, bins, maxr).output(std::string(argv[4]));
  }
  return 0;
}

#include <iostream>
#include <string>

#include "Base.hpp"
#include "StructureFactor.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 4) && (argc != 5) && (argc != 6)) {
    std::cout << "input number should be 2 3 4, scalar mode off by default" << std::endl;
    std::cout << "sk input.xyz 10(window) 3.14(kmax) or \n"
              << "sk input.xyz 10(window) 3.14(kmax) output.sk.xyz\n"
              << "sk input.xyz 10(window) 3.14(kmax) output.sk.xyz 100"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  int window = std::stoi(argv[2]);
  double maxq = std::stod(argv[3]);
  size_t bins = 0;
  if (argc == 6) {
    bins = std::stoul(argv[5]);
  }
  auto data = Frame();

  data.read(filename);

  if (argc == 4) {
    std::string outputName = filename + ".sk.xyz";
    if (data.is2D())
      sk2d(data, window, maxq).output(outputName);
    else if (data.is3D())
      sk3d(data, window, maxq).output(outputName);
  } else if (argc == 5) {
    if (data.is2D())
      sk2d(data, window, maxq).output(std::string(argv[4]));
    else if (data.is3D())
      sk3d(data, window, maxq).output(std::string(argv[4]));
  } else {
    if (data.is2D())
      sk2d_scalar(data, window, maxq, bins).output(std::string(argv[4]));
    else if (data.is3D())
      sk3d_scalar(data, window, maxq, bins).output(std::string(argv[4]));
  }
  return 0;
}

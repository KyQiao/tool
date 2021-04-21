#include <iostream>
#include <string>

#include "Base.hpp"
#include "StructureFactor.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 4) && (argc != 5)) {
    std::cout << "input number should be one or two" << std::endl;
    std::cout << "sk input.xyz 10(window) 3.14(kmax) or \n"
              << "sk input.xyz 10(window) 3.14(kmax) output.sk.xyz"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  int window = std::stoi(argv[2]);
  double maxq = std::stod(argv[3]);

  auto data = Frame();

  data.read(filename);

  if (argc == 4) {
    std::string outputName = filename + ".sk.xyz";
    if (data.is2D())
      sk2d(data, window, maxq).output(outputName);
    else if (data.is3D())
      sk3d(data, window, maxq).output(outputName);
  } else {
    if (data.is2D())
      sk2d(data, window, maxq).output(std::string(argv[4]));
    else if (data.is3D())
      sk3d(data, window, maxq).output(std::string(argv[4]));
  }
  return 0;
}

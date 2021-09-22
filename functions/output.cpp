#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Base.hpp"

int main(int argc, char const *argv[]) {
  if ((argc != 3) && (argc != 4)) {
    std::cout << "input number should be more than 2" << std::endl;
    std::cout << "output input.xyz attr_string or \n"
              << "outputXYZ input.xyz attr_string output.xyz\n"
              << "e.g. output input.xyz \"x y z\" outputfile.xyz "
              << std::endl;
  }

  std::string filename = argv[1];
  auto data = Frame();
  data.read(filename);

  std::string attr_string = argv[2], tmp;
  std::stringstream ss;
  ss << attr_string;
  std::vector<std::string> attrV;

  while (std::getline(ss, tmp, ' ')) {
    attrV.push_back(tmp);
    if (data.attr_index.count(tmp) == 0)
      throw std::runtime_error("attribute not hold!Use describe to see what it holds!");
  }

  Table out;
  for (auto &str : attrV) {
    out.push_back(data.attr_table[data.attr_index[str]]);
  }
  out.setcols(attrV.size());
  out.setrows(data.particleN);


  std::fstream outfile;
  std::string outputName;

  if (argc == 3) {
    outputName = filename + ".xyzOnly.xyz";
  } else {
    outputName = std::string(argv[3]);
  }
  outfile.open(outputName, std::ios::out | std::ios::app);
  outfile << attr_string << "\n";
  outfile.close();
  out.output(outputName);
  return 0;
}

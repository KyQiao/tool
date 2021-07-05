#include <fstream>
#include <iostream>
#include <string>

#include "Base.hpp"

int main(int argc, char const* argv[]) {
  if (argc != 4) {
    std::cout << "count the number of seleted particles" << std::endl;
    std::cout << "input number should be three" << std::endl;
    std::cout << "count_select input.xyz \"c_ackland == 2 \" filename "
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  std::string condition = argv[2];
  std::string outfileName = argv[3];

  auto data = Frame();

  data.read(filename);
  data.select(condition);

  std::ofstream outfile;
  outfile.open(outfileName, std::ios::out | std::ios::app);
  outfile << data.timestep << " " << data.particleN << "\n";
  outfile.close();
  return 0;
}

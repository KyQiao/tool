#include <iostream>
#include <string>

#include "Base.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 3)) {
    std::cout << "input number should be two" << std::endl;
    std::cout << "gr input.xyz input2.xyz"
              << std::endl;
    return 0;
  }

  std::string filename1 = argv[1];
  std::string filename2 = argv[2];

  auto data1 = Frame();
  data1.read(filename1);

  auto data2 = Frame();
  data2.read(filename2);

  auto d1 = data1.get("q");
  auto d2 = data2.get("q");

  double sum = 0, consist = 0;
  double tmp;
  // check consistency
  if (d1.size() != d2.size()) {
    std::cout << "particle number not equil!" << std::endl;
  } else {
    for (size_t i = 0; i < d1.size(); i++) {
      tmp = d1[i] - d2[i];
      consist += tmp;
      sum += std::abs(tmp);
    }
    std::cout << "check swap consistency: " << consist << "should be 0\n"
              << "check swap magnitude:" << sum/d1.size() << std::endl;
  }
  return 0;
}

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Base.hpp"

void testDisp(std::string s1, std::string s2) {
  auto data1 = Frame();
  data1.read(s1);
  auto data2 = Frame();
  data2.read(s2);
  Disp disp = data2 - data1;
  Table out;
  out.setcols(3);
  out.setrows(data1.particleN);
  out.data[0] = std::move(disp.dx);
  out.data[1] = std::move(disp.dy);
  out.data[2] = std::move(disp.dz);
  out.output("testDisp" + std::to_string(disp.dt) + ".xyz");
}

int main(int argc, char const *argv[]) {
  std::string f1 = argv[1], f2 = argv[2];
  testDisp(f1, f2);
  return 0;
}

#include "Frame.hpp"

int main(int argc, char const *argv[]) {
  Frame test;
  // Frame::compressType gz=gz;
  test.readLammps("/mnt/d/data/Simulation/data/atom.0.xyz.gz", Frame::compressType::gz);
  test.describe();

  // test.readLammps("/mnt/d/data/Simulation/data/atom.0.xyz", Frame::compressType::none);
  // test.describe();

  test.attr_table.output("test.txt");
  return 0;
}

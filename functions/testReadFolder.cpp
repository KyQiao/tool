#include <string>

#include "Data.hpp"

void testIO(std::string s) {
  Data data(s);
}

void testIO_gz(std::string s) {
  Data data;
  data.readFolder(s, "atom.*.xyz.gz");
}

void testMatch(std::string s) {
  Data data;
  data.readFolder(s, "*.restart");
  data.readFolder(s, "restart.*");
}

void testLoad(std::string s) {
  Data data;
  data.readFolder(s);
  data.load(1000);
}

int main(int argc, char const *argv[]) {
  std::string path;

  if (argc == 2) {
    path = argv[1];
  } else {
    std::cout << "wrong input entry!" << std::endl;
    return 0;
  }

  // testIO(path);
  // testIO_gz(path);
  // testMatch(path);
  testLoad(path);
  return 0;
}

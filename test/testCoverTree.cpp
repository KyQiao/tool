#include "Frame.hpp"

int main(int argc, char const* argv[]) {
  std::string filename = argv[1];
  auto data = Frame();
  std::cout << "reading file: " << filename << std::endl;
  data.read(filename);

  size_t i = std::stoi(argv[2]);

  // const double query_pt[3] = {
  //     data.boxXL + data.xl / 2,
  //     data.boxYL + data.yl / 2,
  //     data.boxZL + data.zl / 2,
  // };
  // const double query_pt[3]
  //     = {
  //         data.x[i],
  //         data.y[i],
  //         data.z[i],
  //     };

  data.buildCoverTree();
  auto p = data.data2Point(i);
  auto res = data.cTree.kNearestNeighbors(p, 13);

  for (auto& v : res) {
    std::cout << "id=" << v.getId() << " "
              << " dist=" << p.distance(v) << std::endl;
  }
  return 0;
}
#include <omp.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>

#include "Frame.hpp"
#include "Table.hpp"
#include "vptree.hpp"

// void cubic2_pbc_test() {
//   using point_t = std::vector<int>;

//   auto cutoff_radius = 1.2;
//   std::vector<int> dimensions = {2, 2, 2};
//   std::vector<bool> periodicity = {true, true, true};

//   auto points = std::vector<point_t>{
//       {0, 0, 0},
//       {0, 0, 1},
//       {0, 1, 0},
//       {0, 1, 1},
//       {1, 0, 0},
//       {1, 1, 0},
//       {1, 1, 1},
//       {1, 0, 1}};

//   auto metric = vpt::EuclideanMetricPbc<point_t, std::vector<int>, std::vector<bool>>(dimensions, periodicity);
//   auto tree = vpt::VpTree<std::vector<int>>(points, metric);

//   auto [treeDistances, treeIndices] = tree.getNeighborhoodPoints(points[0], cutoff_radius);

//   std::vector<int> benchmark_treeIndices = {0, 1, 2, 4};

//   std::cout << std::boolalpha
//             << "cubic2_pbc_test: "
//             << (std::is_permutation(treeIndices.begin(), treeIndices.end(), benchmark_treeIndices.begin(), benchmark_treeIndices.end()))
//             << std::endl;
// }

int main(int argc, char const* argv[]) {
  std::string filename = argv[1];
  auto data = Frame();
  std::cout << "reading file: " << filename << std::endl;
  data.read(filename);
  // size_t ind = std::stoi(argv[2]);

  // Table points{data.x, data.y, data.z};
  using point_t = std::vector<double>;
  std::vector<point_t> points;
  for (size_t i = 0; i < data.particleN; i++) {
    points.push_back({data.x[i], data.y[i], data.z[i]});
  }

  std::vector<bool> periodicity = {true, true, true};
  std::vector<double> dimensions = {data.xl, data.yl, data.zl};
  double cutoff_radius = 2;

  // cpp17 version
  // auto metric = vpt::EuclideanMetricPbc<point_t, std::vector<double>, std::vector<bool>>(dimensions, periodicity);
  // auto tree = vpt::VpTree<point_t>(points, metric);
  // auto res = tree.getNeighborhoodPoints(points[ind], cutoff_radius);

  // cpp11 version

  auto metric = vpt::EuclideanMetricPbc<point_t>(dimensions, periodicity);
  auto tree = vpt::VpTree(points, metric);

#pragma parallel for
  for (size_t j = 0; j < 100; j++) {
    auto res = tree.getNeighborhoodPoints(points[j], cutoff_radius);
    std::cout << "get" << res.first.size() - 1 << "neighbors" << std::endl;
  }
}

// std::cout << std::boolalpha
//           << "cubic2_pbc_test: "
//           << (std::is_permutation(treeIndices.begin(), treeIndices.end(), benchmark_treeIndices.begin(), benchmark_treeIndices.end()))
//           << std::endl;
// return 0;

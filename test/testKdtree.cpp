#include <iostream>

#include "Frame.hpp"
#include "kdtree.hpp"

int main(int argc, char const *argv[]) {
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
  const double query_pt[3]
      = {
          data.x[i],
          data.y[i],
          data.z[i],
      };

  // data.describe();

  const FrameAdaptor ada(data);

  nanoflann::SearchParams params;
  //params.sorted = false;
  FrameKdTree index(3, ada, KDTreeSingleIndexAdaptorParams(10));
  index.buildIndex();

  FrameKdTreePBC indexPBC(3, ada, KDTreeSingleIndexAdaptorParams(10));
  indexPBC.buildIndex();
  // const size_t num_results = 1;
  // size_t ret_index;
  // double out_dist_sqr;
  // nanoflann::KNNResultSet<double> resultSet(num_results);
  // resultSet.init(&ret_index, &out_dist_sqr);
  // index.findNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10));
  // //index.knnSearch(query, indices, dists, num_results, mrpt_flann::SearchParams(10));

  // std::cout << "knnSearch(nn=" << num_results << "): \n";
  // std::cout << "ret_index=" << ret_index << " out_dist_sqr=" << out_dist_sqr << std::endl;

  {
    const double search_radius = static_cast<double>(3);
    std::vector<std::pair<size_t, double>> ret_matches;

    const size_t nMatches = index.radiusSearch(&query_pt[0], search_radius, ret_matches, params);

    std::cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
    for (size_t i = 0; i < nMatches; i++)
      std::cout << "idx[" << i << "]=" << ret_matches[i].first << " dist[" << i << "]=" << ret_matches[i].second << std::endl;
    std::cout << "\n";
  }

  {
    const double search_radius = static_cast<double>(3);
    std::vector<std::pair<size_t, double>> ret_matches;

    const size_t nMatches = indexPBC.radiusSearch(&query_pt[0], search_radius, ret_matches, params);

    std::cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
    for (size_t i = 0; i < nMatches; i++)
      std::cout << "idx[" << i << "]=" << ret_matches[i].first << " dist[" << i << "]=" << ret_matches[i].second << std::endl;
    std::cout << "\n";
  }
  return 0;
}

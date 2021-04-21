#include <omp.h>

#include <cmath>
#include <iostream>
#include <string>

#include "Base.hpp"
#include "Table.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 3) && (argc != 4)) {
    std::cout << "input number should be two or three" << std::endl;
    std::cout << "densityProfile input.xyz 100\n"
              << "densityProfile input.xyz 100 output.dat"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  int bins = std::stoi(std::string(argv[2]));

  auto data = Frame();
  data.read(filename);

  const double binsize = (data.boxZH - data.boxZL) / bins;
  std::vector<double> hist(bins), binedge(bins);
  omp_lock_t* hist_locks = new omp_lock_t[bins];

  std::cout << "setup finished" << std::endl;

  // setup lock for hist
#pragma omp parallel for
  for (int i = 0; i < bins; i++) {
    omp_init_lock(&hist_locks[i]);
  }

  int index;
#pragma omp parallel for private(index)
  for (size_t i = 0; i < data.particleN; i++) {
    index = static_cast<int>((data.z[i] - data.boxZL) / binsize);
    omp_set_lock(&hist_locks[index]);
    hist[index] += 1;
    omp_unset_lock(&hist_locks[index]);
  }

  // for (size_t i = 0; i < data.particleN; i++) {
  //   index = static_cast<int>((data.z[i] - data.boxZL) / binsize);
  //   hist[index] += 1;
  // }

  std::cout << "cal finished" << std::endl;

  for (size_t i = 0; i < binedge.size(); i++) {
    binedge[i] = (i + 0.5) * binsize;
    hist[i] /= data.particleN;
  }

  // Table out = Table({binedge, hist});

  std::cout << "table finished" << std::endl;

  if (argc == 3) {
    std::string outputName = filename + ".profile.xyz";
    Table({binedge, hist}).output(outputName);
    // out.output(outputName);
  } else {
    Table({binedge, hist}).output(std::string(argv[3]));
    // out.output(std::string(argv[3]));
  }
  return 0;
}

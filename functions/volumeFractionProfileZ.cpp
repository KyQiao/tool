// calculate the volume fraction of sediment.
// by default the type 1 is 1 sigma and type 2 is 1.4 sigma.
// later we can add size property to frame.

#include <omp.h>

#include <cmath>
#include <iostream>
#include <string>

#include "Base.hpp"
#include "Table.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 3) && (argc != 4)) {
    std::cout << "input number should be two or three" << std::endl;
    std::cout << "volumeFractionProfileZ input.xyz 100\n"
              << "volumeFractionProfileZ input.xyz 100 output.dat"
              << std::endl;
    return 0;
  }

  std::string filename = argv[1];
  int bins = std::stoi(std::string(argv[2]));

  Frame data = Frame();
  data.read(filename);

  const double binsize = (data.boxZH - data.boxZL) / bins;
  const double binVolume = binsize * data.xl * data.yl;
  const double fourThirdPi_d8 = 4.1887902047863905 / 8;
  const double ratio = fourThirdPi_d8 / binVolume;
  std::vector<double> hist(bins), binedge(bins);
  omp_lock_t* hist_locks = new omp_lock_t[bins];

  std::cout << "setup finished" << std::endl;

  // setup lock for hist
#pragma omp parallel for
  for (int i = 0; i < bins; i++) {
    omp_init_lock(&hist_locks[i]);
  }

  int index;

  if (data.attr_index.count("q")) {
    size_t tmp = data.attr_index["q"];
#pragma omp parallel for private(index)
    for (size_t i = 0; i < data.particleN; i++) {
      index = static_cast<int>((data.z[i] - data.boxZL) / binsize);
      omp_set_lock(&hist_locks[index]);
      // when size info is stored
      hist[index] += std::pow(data.attr_table.data[tmp][i], 3);
      omp_unset_lock(&hist_locks[index]);
    }
  } else {
#pragma omp parallel for private(index)
    for (size_t i = 0; i < data.particleN; i++) {
      index = static_cast<int>((data.z[i] - data.boxZL) / binsize);
      omp_set_lock(&hist_locks[index]);
      // by default the type 1 is 1 sigma and type 2 is 1.4 sigma.
      if (data.type[i] == 1)
        hist[index] += 1;
      else
        hist[index] += 2.744;  //1.4^3
      omp_unset_lock(&hist_locks[index]);
    }
  }
  // for (size_t i = 0; i < data.particleN; i++) {
  //   index = static_cast<int>((data.z[i] - data.boxZL) / binsize);
  //   hist[index] += 1;
  // }

  std::cout << "cal finished" << std::endl;

  for (size_t i = 0; i < binedge.size(); i++) {
    binedge[i] = (i + 0.5) * binsize;
    hist[i] *= ratio;
  }

  // Table out = Table({binedge, hist});

  std::cout << "table finished" << std::endl;

  if (argc == 3) {
    std::string outputName = filename + ".volumeFractionProfile.xyz";
    Table({binedge, hist}).output(outputName);
    // out.output(outputName);
  } else {
    Table({binedge, hist}).output(std::string(argv[3]));
    // out.output(std::string(argv[3]));
  }
  return 0;
}

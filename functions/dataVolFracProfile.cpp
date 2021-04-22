// calculate the volume fraction .
//

#include <omp.h>

#include <cmath>
#include <iostream>
#include <string>

#include "Base.hpp"
#include "Table.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 3) && (argc != 4)) {
    std::cout << "input number should be two or three" << std::endl;
    std::cout << "volumeFractionProfileZ dataFolder 100\n"
              << "volumeFractionProfileZ dataFolder 100 output.dat"
              << std::endl;
    return 0;
  }

  std::string folder = argv[1];
  int bins = std::stoi(std::string(argv[2]));

  Data data(folder);
  // by default load all file in folder
  data.load();

  const double binsize = (data.front()->boxZH - data.front()->boxZL) / bins;
  const double binVolume = binsize * data.front()->xl * data.front()->yl;
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

  for (size_t i = 0; i < data.size(); i++) {
    int index;

    if (data.front()->attr_index.count("q")) {
      size_t tmp = data.front()->attr_index["q"];
#pragma omp parallel for private(index)
      for (size_t i = 0; i < data.front()->particleN; i++) {
        index = static_cast<int>((data.front()->z[i] - data.front()->boxZL) / binsize);
        omp_set_lock(&hist_locks[index]);
        // when size info is stored
        hist[index] += std::pow(data.front()->attr_table.data[tmp][i], 3);
        omp_unset_lock(&hist_locks[index]);
      }
    } else {
#pragma omp parallel for private(index)
      for (size_t i = 0; i < data.front()->particleN; i++) {
        index = static_cast<int>((data.front()->z[i] - data.front()->boxZL) / binsize);
        omp_set_lock(&hist_locks[index]);
        // by default the type 1 is 1 sigma and type 2 is 1.4 sigma.
        if (data.front()->type[i] == 1)
          hist[index] += 1;
        else
          hist[index] += 2.744;  //1.4^3
        omp_unset_lock(&hist_locks[index]);
      }
    }
    // for (size_t i = 0; i < data.front()->particleN; i++) {
    //   index = static_cast<int>((data.front()->z[i] - data.front()->boxZL) / binsize);
    //   hist[index] += 1;
    // }
  }
  std::cout << "cal finished" << std::endl;

  for (size_t i = 0; i < binedge.size(); i++) {
    binedge[i] = (i + 0.5) * binsize;
    hist[i] *= ratio / data.size();
  }

  // Table out = Table({binedge, hist});

  std::cout << "table finished" << std::endl;

  fs::path dirName = fs::u8path(folder);

  if (argc == 3) {
    std::string outputName = dirName.filename().string() + ".volFracProf.xyz";
    Table({binedge, hist}).output(outputName);
    // out.output(outputName);
  } else {
    Table({binedge, hist}).output(std::string(argv[3]));
    // out.output(std::string(argv[3]));
  }
  return 0;
}

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
  fs::path dirName = fs::u8path(folder);

  if (!fs::is_directory(dirName)) {
    std::cout << "input folder name is invalid!" << std::endl;
    return 0;
  }

  int bins = std::stoi(std::string(argv[2]));

  Data data(folder);
  // by default load all file in folder
  data.load();

  const double binsize = (data.front()->boxYH - data.front()->boxYL) / bins;
  // now area
  const double binVolume = binsize * data.front()->xl;
  // const double fourThirdPi_d8 = 4.1887902047863905 / 8;
  // 2d system
  const double oneFourthPi = 3.1415926535 / 4;
  const double ratio = oneFourthPi / binVolume;

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
        index = static_cast<int>((data.front()->y[i] - data.front()->boxYL) / binsize);
        omp_set_lock(&hist_locks[index]);
        // when size info is stored
        hist[index] += std::pow(data.front()->attr_table.data[tmp][i], 2);
        omp_unset_lock(&hist_locks[index]);
      }
    } else {
#pragma omp parallel for private(index)
      for (size_t i = 0; i < data.front()->particleN; i++) {
        index = static_cast<int>((data.front()->y[i] - data.front()->boxYL) / binsize);
        omp_set_lock(&hist_locks[index]);
        // by default the type 1 is 1 sigma and type 2 is 1.4 sigma.
        if (data.front()->type[i] == 1)
          hist[index] += 1;
        else
          hist[index] += 1.96;  //1.4^2
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

  if (argc == 3) {
    std::string outputName = dirName.parent_path().filename().string() + ".volFracProf2d.xyz";
    Table({binedge, hist}).output(outputName);
    // out.output(outputName);
  } else {
    Table({binedge, hist}).output(std::string(argv[3]));
    // out.output(std::string(argv[3]));
  }
  return 0;
}

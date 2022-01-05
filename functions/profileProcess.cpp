// calculate the volume fraction .
//

#include <omp.h>

#include <cmath>
#include <iostream>
#include <string>

#include "Base.hpp"
#include "Table.hpp"

int main(int argc, char const* argv[]) {
  if ((argc != 7) && (argc != 5)) {
    std::cout << "input number should be 4 or 6" << std::endl;
    std::cout << "profileProcess dataFolder slicesForHist avgNumber  outputPattern StartFrame EndFrame\n"
              << "profileProcess ./T0004_05 100 10  profile 0 3000\n"
              << "profileProcess ./T0004_05 100 10  profile \n"
              << std::endl;
    return 0;
  }

  std::string folder = argv[1];
  fs::path dirName = fs::u8path(folder);

  int avgNumber = std::stoi(argv[3]);
  std::string pattern = argv[4];
  size_t start = 0, end = -1;

  if (!fs::is_directory(dirName)) {
    std::cout << "input folder name is invalid!" << std::endl;
    return 0;
  }

  int bins = std::stoi(std::string(argv[2]));

  Data data(folder);

  if (argc == 7) {
    start = std::stoi(argv[5]);
    end = std::stoi(argv[6]);
  } else {
    end = data.fileIndex.size();
  }

  // setup left zero
  size_t NumberOfOutput = static_cast<size_t>(std::ceil((end - start) / double(avgNumber)));
  size_t NumberOfNumber = 1;
  while (NumberOfOutput / 10 > 1) {
    NumberOfNumber += 1;
    NumberOfOutput /= 10;
  }

  // the high will not be included
  // load 1 file to get const info
  data.load(1, start, start + 1);
  const double binsize = (data.front()->boxZH - data.front()->boxZL) / bins;
  const double binVolume = binsize * data.front()->xl * data.front()->yl;
  const double fourThirdPi_d8 = 4.1887902047863905 / 8;
  const double ratio = fourThirdPi_d8 / binVolume;
  // setup container for data
  std::vector<double> hist(bins), binedge(bins);
  omp_lock_t* hist_locks = new omp_lock_t[bins];

  size_t SavingIndex = 1;

  for (size_t i = start; i < end; i += avgNumber) {
    data.clear();
    data.load(1, i, i + avgNumber);

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
          hist[index] += std::pow(data.front()->attr_table[tmp][i], 3);
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

    for (size_t i = 0; i < binedge.size(); i++) {
      binedge[i] = (i + 0.5) * binsize;
      hist[i] *= ratio / data.size();
    }

    std::string Number = std::to_string(SavingIndex);
    std::string FinalNumber = std::string(NumberOfNumber - Number.length(), '0') + Number;
    std::string outputName = pattern + "." + FinalNumber + ".xyz";

    SavingIndex += 1;

    Table({binedge, hist}).output(outputName);
  }
  return 0;
}

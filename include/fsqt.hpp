
#pragma once
#include <cmath>

#include "Data.hpp"
#include "besj0.hpp"
#include "lebedev.hpp"
#ifdef _OPENMP
#include "omp.h"
#endif
#include "type.hpp"

#define debugFsqt 0

class Fsqt {
public:
  std::vector<size_t> dts{0ul};       // the first one is zero
  std::vector<size_t> true_dts{0ul};  // the one return value
  size_t dt = 1, k = 1;
  std::vector<size_t> counts{1};
  std::vector<double> res{1};      // data container
  std::vector<double> x, y, z, w;  //weight and sample points
  double q;
  size_t deltaF, nframe;

  bool is2d = false;

#ifdef _OPENMP
  omp_lock_t *hist_locks = nullptr;
#endif

  ~Fsqt(){};

  Fsqt(const Data &data, double q) :
      q(q) {
    if (data.empty()) {
      // something wrong
    }
    deltaF = std::abs(data.fileIndex[1] - data.fileIndex[0]);
    nframe = data.size();

    while (dt < nframe) {
      if (dt != dts.back()) {
        dts.push_back(dt);
        true_dts.push_back(dt * deltaF);
      }
      // array([0,1,2,3,4,
      //      5, 6, 7, 8, 9,
      //     11, 12, 14, 16, 19,
      //     22, 25, 29, 33, 38,
      //     44, 50, 58, 66, 76,
      //     88, 101, 116, 133, 153,
      //     176, 203, 233, 268, 308,
      //     354, 407, 468, 539, 620,
      //     713, 819, 942, 1084, 1246,
      //     1433, 1648, 1895, 2180, 2507,
      //     2883, 3315, 3812, 4384, 5042,
      dt = static_cast<size_t>(std::exp(std::log(1.15) * (k++)) + 0.5);
    }
    counts.resize(dts.size());
    res.resize(dts.size());
    if (data.is2D()) {
      is2d = true;
      set2D();
    } else {
      set3D();
    }

#if debugFsqt
    std::cout << "setup successful" << std::endl;
#endif

    // set up locks

#ifdef _OPENMP
    hist_locks = new omp_lock_t[dts.size()];

    for (size_t i = 0; i < dts.size(); i++) {
      omp_init_lock(&hist_locks[i]);
    }
#endif

#pragma omp parallel for
    for (size_t firstFrame = 0; firstFrame < nframe; ++firstFrame) {
      // Frame *fold = data.at(firstFrame);
#if debugFsqt
      std::cout << "process frame " << firstFrame << std::endl;
#endif

      for (size_t secondFrame = 1; secondFrame < dts.size() && firstFrame + dts[secondFrame] < nframe; ++secondFrame) {
#if debugFsqt
        // std::cout << "setup successful" << std::endl;
        std::cout << '(' << secondFrame << ',' << firstFrame << ',' << firstFrame + dts[secondFrame] << ')' << std::endl;
#endif
        // update(secondFrame, *data[firstFrame] - *data[firstFrame + dts[secondFrame]]);

        double tmp = 0;
        const Disp &disp = *data[firstFrame] - *data[firstFrame + dts[secondFrame]];
        for (size_t indexSample = 0; indexSample < w.size(); indexSample++) {
          // if (isMonotype) {  // compiler should choose which one
          dtype ds = 0.0;
          for (size_t ii = 0; ii < disp.dx.size(); ii++) {
            ds = x[indexSample] * disp.dx[ii];
            ds += y[indexSample] * disp.dy[ii];
            if (!is2d)
              ds += z[indexSample] * disp.dz[ii];
            tmp += w[indexSample] * std::cos(this->q * ds);
          }
        }
        tmp /= disp.dx.size();

#ifdef _OPENMP
        omp_set_lock(&hist_locks[secondFrame]);
#if debugFsqt
        std::cout << std::to_string(secondFrame) + " locked\n";
#endif
#endif
        res[secondFrame] += tmp;
        counts[secondFrame] += 1;
#ifdef _OPENMP
        omp_unset_lock(&hist_locks[secondFrame]);
#if debugFsqt
        std::cout << std::to_string(secondFrame) + " unlocked\n";
#endif
#endif

        // #pragma omp atomic
      }
    }

    cleanup();
#ifdef _OPENMP

    for (size_t i = 0; i < dts.size(); i++) {
      omp_destroy_lock(&hist_locks[i]);
    }
    delete[] hist_locks;
#endif
  };

  // move constructor
  // https://stackoverflow.com/questions/35830393/c-move-constructor-for-class-containing-vector
  Fsqt(Fsqt &&dp) = default;
  Fsqt(const Fsqt &dp) = delete;

  // update the data for each cf1 - cf2
  void update(const size_t &index, const Disp &disp) {
    // loop for all sample vector
    double tmp = 0;
    for (size_t indexSample = 0; indexSample < w.size(); indexSample++) {
      // if (isMonotype) {  // compiler should choose which one
      dtype ds = 0.0;
      for (size_t i = 0; i < disp.dx.size(); i++) {
        ds = x[indexSample] * disp.dx[i];
        ds += y[indexSample] * disp.dy[i];
        if (!is2d)
          ds += z[indexSample] * disp.dz[i];
        tmp += w[indexSample] * std::cos(this->q * ds);
      }
    }
    tmp /= disp.dx.size();
    // #pragma omp atomic
    res[index] += tmp;
    // #pragma omp atomic
    counts[index] += 1;
  };
  // clean up the results, e.g. perform mean/std operation
  void cleanup() {
    // start from 1 since the first value should always be 1
    for (size_t i = 1; i < res.size(); i++) {
      res[i] /= counts[i];
    }
  }

  void set2D() {
    // try 50 first
    int N = 50;
    const double PI = acos(-1);
    x.resize(N);
    y.resize(N);
    w.resize(N, 1.0 / N);
    for (size_t i = 0; i < N; i++) {
      x[i] = cos(i * 2 * PI / N);
      y[i] = sin(i * 2 * PI / N);
    }
  }

  void set3D() {
    // the number must be a prior choosen from LDNS
    // 6, 14, 26, 38, 50, 74, 86, 110, 146, 170, 194, 230, 266, 302, 350, 434,
    // 590, 770, 974, 1202, 1454, 1730, 2030, 2354, 2702, 3074, 3470, 3890,
    // 4334, 4802, 5294, 5810

    // try 50 first
    int N = 50;

    x.resize(N);
    y.resize(N);
    z.resize(N);
    w.resize(N);
    ld(x.data(), y.data(), z.data(), w.data(), &N);
  }

  // not same type
  // cannot bind together
  // Table out() {
  //   return Table({dts, res, counts});
  // }
};

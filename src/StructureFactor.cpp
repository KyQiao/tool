#include "StructureFactor.hpp"

#include <omp.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "Base.hpp"

const double PI = std::acos(-1);

Table sk2d(const Frame& data, size_t window, double kmax) {
  double dkx = 2 * PI / (data.boxXH - data.boxXL);
  double dky = 2 * PI / (data.boxYH - data.boxYL);

  std::vector<dtype> kx, ky, sk;
  // choose maxk and window size, calculated mesh of vector.
  double k = 0;
  size_t xMaxIndex = static_cast<size_t>((kmax / dkx) / window);
  size_t yMaxIndex = static_cast<size_t>((kmax / dky) / window);

  for (size_t i = 0; i < xMaxIndex; i++) {
    for (size_t j = 0; j < yMaxIndex; j++) {
      k = std::sqrt(dkx * dkx * window * i * window * i
                    + dky * dky * window * j * window * j);
      if (k < kmax) {
        kx.push_back(dkx * window * i);
        ky.push_back(dky * window * j);
      }
    }
  }

  sk.resize(kx.size());
  double sumOfCos = 0, sumOfSin = 0;

#pragma omp parallel for firstprivate(sumOfCos, sumOfSin)
  for (size_t i = 0; i < kx.size(); i++) {
    for (size_t j = 0; j < data.particleN; j++) {
      sumOfCos += std::cos(kx[i] * data.x[j] + ky[i] * data.y[j]);
      sumOfSin += std::sin(kx[i] * data.x[j] + ky[i] * data.y[j]);
    }
    sk[i] = std::pow(sumOfCos, 2) + std::pow(sumOfSin, 2);
    sk[i] /= data.particleN;
    sumOfCos = 0;
    sumOfSin = 0;
  }
  return Table({kx, ky, sk});
}

Table sk3d(const Frame& data, size_t window, double kmax) {
  double dkx = 2 * PI / (data.boxXH - data.boxXL);
  double dky = 2 * PI / (data.boxYH - data.boxYL);
  double dkz = 2 * PI / (data.boxZH - data.boxZL);

  std::vector<dtype> kx, ky, kz, sk;
  // choose maxk and window size, calculated mesh of vector.
  double k = 0;
  size_t xMaxIndex = static_cast<size_t>((kmax / dkx) / window);
  size_t yMaxIndex = static_cast<size_t>((kmax / dky) / window);
  size_t zMaxIndex = static_cast<size_t>((kmax / dkz) / window);
  for (size_t i = 0; i < xMaxIndex; i++) {
    for (size_t j = 0; j < yMaxIndex; j++) {
      for (size_t l = 0; l < zMaxIndex; l++) {
        k = std::sqrt(dkx * dkx * window * i * window * i
                      + dky * dky * window * j * window * j
                      + dkz * dkz * window * l * window * l);
        if (k < kmax) {
          kx.push_back(dkx * window * i);
          ky.push_back(dky * window * j);
          kz.push_back(dkz * window * l);
        }
      }
    }
  }

  sk.resize(kx.size());
  double sumOfCos = 0, sumOfSin = 0;

#pragma omp parallel for firstprivate(sumOfCos, sumOfSin)
  for (size_t i = 0; i < kx.size(); i++) {
    for (size_t j = 0; j < data.particleN; j++) {
      sumOfCos += std::cos(kx[i] * data.x[j] + ky[i] * data.y[j] + kz[i] * data.z[j]);
      sumOfSin += std::sin(kx[i] * data.x[j] + ky[i] * data.y[j] + kz[i] * data.z[j]);
    }
    sk[i] = std::pow(sumOfCos, 2) + std::pow(sumOfSin, 2);
    sk[i] /= data.particleN;
    sumOfCos = 0;
    sumOfSin = 0;
  }
  return Table({kx, ky, kz, sk});
}
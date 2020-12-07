// calculate pair distribution function with PBC
// return binedge(mid point of left and right edge) and gr

#include <omp.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "Base.hpp"

const double PI = acos(-1);

inline double periodic_distance(double x, double lx) {
  return (std::abs(x) > 0.5 * lx) ? ((x > 0) ? x - lx : x + lx) : x;
}

Table gr2d(const Frame& data, size_t bins, double maxr) {
  double xl = (data.boxXH - data.boxXL), yl = (data.boxYH - data.boxYL);
  double rmax = std::min(maxr, std::min(xl * 0.5, yl * 0.5));
  const double rmax2 = rmax * rmax;
  const double binsize = rmax / bins;
  std::vector<double> hist(bins), binedge(bins);
  omp_lock_t* hist_locks = new omp_lock_t[bins];

// setup lock for hist
#pragma omp parallel for
  for (size_t i = 0; i < bins; i++) {
    omp_init_lock(&hist_locks[i]);
  }

  double dx, dy, d;
  int index;
#pragma omp parallel for private(dx, dy, d, index)
  for (size_t i = 0; i < data.particleN; i++) {
    for (size_t j = i + 1; j < data.particleN; j++) {
      dx = periodic_distance(data.x[i] - data.x[j], xl);
      d = dx * dx;
      if (d < rmax2) {
        dy = periodic_distance(data.y[i] - data.y[j], yl);
        d += dy * dy;
        if (d < rmax2) {
          index = static_cast<int>(std::sqrt(d) / binsize);
          omp_set_lock(&hist_locks[index]);
          hist[index] += 2;
          omp_unset_lock(&hist_locks[index]);
        }
      }
    }
  }

  // free the lock
#pragma omp parallel for
  for (size_t i = 0; i < bins; i++)
    omp_destroy_lock(&hist_locks[i]);
  delete[] hist_locks;
  const double V = xl * yl;
  const double density = data.particleN / V;

  for (size_t i = 0; i < bins; ++i) {
    binedge[i] = (i + 0.5) * binsize;
    hist[i] /= PI * (2. * i + 1.) * binsize * binsize * density * density * V;
  }
  return Table({binedge, hist});
}

Table gr3d(const Frame& data, size_t bins, double maxr) {
  double xl = (data.boxXH - data.boxXL), yl = (data.boxYH - data.boxYL), zl = (data.boxZH - data.boxZL);
  double rmax = std::min(maxr, std::min(zl * 0.5, std::min(xl * 0.5, yl * 0.5)));
  const double rmax2 = rmax * rmax;
  const double binsize = rmax / bins;
  std::vector<double> hist(bins), binedge(bins);
  omp_lock_t* hist_locks = new omp_lock_t[bins];

// setup lock for hist
#pragma omp parallel for
  for (size_t i = 0; i < bins; i++) {
    omp_init_lock(&hist_locks[i]);
  }

  double dx, dy, dz, d;
  int index;
#pragma omp parallel for private(dx, dy, dz, d, index)
  for (size_t i = 0; i < data.particleN; i++) {
    for (size_t j = i + 1; j < data.particleN; j++) {
      dx = periodic_distance(data.x[i] - data.x[j], xl);
      d = dx * dx;
      if (d < rmax2) {
        dy = periodic_distance(data.y[i] - data.y[j], yl);
        d += dy * dy;
        if (d < rmax2) {
          dz = periodic_distance(data.z[i] - data.z[j], zl);
          d += dz * dz;
          if (d < rmax2) {
            index = static_cast<int>(std::sqrt(d) / binsize);
            omp_set_lock(&hist_locks[index]);
            hist[index] += 2;
            omp_unset_lock(&hist_locks[index]);
          }
        }
      }
    }
  }
// free the lock
#pragma omp parallel for
  for (size_t i = 0; i < bins; i++)
    omp_destroy_lock(&hist_locks[i]);
  delete[] hist_locks;

  const double V = xl * yl * zl;
  const double density = data.particleN / V;
  // keep float point
  const double coff = 4. / 3. * PI * binsize * binsize * binsize * density * density * V;

  for (size_t i = 0; i < bins; ++i) {
    binedge[i] = (i + 0.5) * binsize;
    hist[i] /= (1. + 3. * i * i + 3. * i) * coff;
  }
  return Table({binedge, hist});
}

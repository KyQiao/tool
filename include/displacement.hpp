#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include <cmath>

#include "Data.hpp"
#include "besj0.hpp"
#include "lebedev.hpp"
#include "omp.h"
#include "type.hpp"

class DispProperty {
public:
  const size_t ntype;
  const int *typeIDs;
  std::vector<size_t> dts;
  std::vector<size_t> counts;
  std::vector<double> res;  // data container

  DispProperty(const size_t n_dts, const size_t n_counts, const size_t n_res,
               const size_t ntype_) :
      ntype(ntype_),
      typeIDs(nullptr), dts(n_dts), counts(n_counts),
      res(n_res) {}

  // move constructor
  // https://stackoverflow.com/questions/35830393/c-move-constructor-for-class-containing-vector
  DispProperty(DispProperty &&dp) = default;
  DispProperty(const DispProperty &dp) = delete;

  // update the data for each cf1 - cf2
  virtual void update(const size_t index, const size_t dt, const Disp &disp){};
  // clean up the results, e.g. perform mean/std operation
  virtual void cleanup(){};
};

/* Compute correlated displacement properties with unified pattern.
       Those properties include
        1. MDS
        2. non-Gaussian parameter
        3. von Hove function
        4. F(q, t)
        5. Q4
    */
template <class T>
T compute_correlated_displacement_property(const Data &data) {
  /* compute possible dts in logrithmic scale, equivalent to the following python code:
           dtindex = np.unique((np.exp(np.log(1.15)*np.arange(300)) + 0.5).astype(np.int))
           assert dtindex[-1] > maxNframe
           dtindex = dtindex[dtindex < maxNframe]
           Note that disp(dt=0)=0, so the result should be calculated analytically */

  // this is generateed by class Data
  // assert_frameIDs_evenly_separated();
  if (data.empty()) {
    T property(0, data[0]->ntype);
    return property;
  }

  const size_t deltaF = std::abs(data.fileIndex[1] - data.fileIndex[0]);
  const size_t nframe = data.size();

// set up locks
#ifdef _OPENMP
  omp_lock_t *hist_locks = new omp_lock_t[nframe];
#pragma omp parallel for
  for (size_t i = 0; i < nframe; i++) {
    omp_init_lock(&hist_locks[i]);
  }
#endif

  std::vector<size_t> dts(1);  // the first one is zero
  size_t dt = 1, k = 1;
  while (dt < nframe) {
    if (dt != dts.back())
      dts.push_back(dt);
    dt = static_cast<size_t>(std::exp(std::log(1.15) * (k++)) + 0.5);
  }

  T property(dts.size(), data[0]->ntype);
  dts.push_back(nframe);  // to make sure the following inner loop stop correctly
#pragma parallel for
  for (size_t i = 0; i < nframe; ++i) {
    Frame *fold = data[i];
    /* The following loop has logical error, since dts[dts.size()] is undefined
               Error: for (k = 1; i+dts[k]<nframe; ++k) { // dts[k=0]=0
               However, k<dts.size() is also incorrect.
               Hence we add the last element of dts to be nframe,
               to make sure the following loop stop correctly.  */
    for (k = 1; i + dts[k] < nframe; ++k) {  // dts[k=0]=0
      Frame *fnew = data[i + dts[k]];
      //std::cout << '(' << k << ',' << i << ',' << i+dts[k] << ')' << std::endl;
      omp_set_lock(&hist_locks[k]);
      property.update(k, dts[k] * deltaF, *fnew - *fold);
      omp_unset_lock(&hist_locks[k]);
    }
  }

#ifdef _OPENMP
#pragma omp parallel for
  for (size_t i = 0; i < nframe; i++)
    omp_destroy_lock(&hist_locks[i]);
  delete[] hist_locks;
#endif

  property.cleanup();
  return property;
}

class MSD_t : public DispProperty {
public:
  /* vector counts shape = (ntype, ndts)
     * vector res shape = (ntype, ndts, 5),
     * for dx_mean, dy_mean, dx_squared, dy_squared, dx_squared+dy_squared;
     */

  MSD_t(const size_t n, const size_t ntype_) :
      // 3d date by default
      DispProperty(n, ntype_ * n, ntype_ * n * 7, ntype_) {}

  /* no need
    ~MSD_t() {
        std::cout << "dts.addr = " << dts.data() << std::endl;
        std::cout << "counts.addr = " << counts.data() << std::endl;
        std::cout << "res.addr = " << res.data() << std::endl;
        dts.clear();
        counts.clear();
        res.clear();
        std::cout << "destructor is called" << std::endl; }
    */

  void update(const size_t index, const size_t dt, const Disp &disp) override {
    // if (isMonotype) {  // compiler should choose which one
    dtype dx_mean = 0.0, dy_mean = 0.0, dx_squared = 0.0, dy_squared = 0.0, dz_mean = 0.0, dz_squared = 0.0;
    bool is3D = false;
    for (size_t i = 0; i < disp.dx.size(); i++) {
      dx_mean += disp.dx[i];
      dy_mean += disp.dy[i];
      dx_squared += disp.dx[i] * disp.dx[i];
      dy_squared += disp.dy[i] * disp.dy[i];
    }
    if (disp.dz.size() > 0) {
      is3D = true;
      for (size_t i = 0; i < disp.dx.size(); i++) {
        dz_mean += disp.dz[i];
        dz_squared += disp.dz[i] * disp.dz[i];
      }
    }

    //if (index >= dts.size())
    //    throw std::runtime_error("wrong index");
    dts[index] = dt;
    double *ptr = res.data() + 7 * index;
    *(ptr++) += dx_mean;
    *(ptr++) += dy_mean;
    *(ptr++) += dz_mean;
    *(ptr++) += dx_squared;
    *(ptr++) += dy_squared;
    *ptr += dz_squared;

    counts[index] += disp.dx.size();
    // non-nomotype will not be used for now
    // } else {
    //   std::vector<float> dx_mean(ntype), dy_mean(ntype), dx_squared(ntype), dy_squared(ntype);
    //   std::vector<size_t> c(ntype);
    //   for (const auto &disp : disp) {
    //     const int typeID = typeIdisp[disp.pIndex];
    //     //std::cout << "typeID = " << typeID << " , " << disp.dx << std::endl;
    //     dx_mean[typeID] += disp.dx;
    //     dy_mean[typeID] += disp.dy;
    //     dx_squared[typeID] += disp.dx * disp.dx;
    //     dy_squared[typeID] += disp.dy * disp.dy;
    //     ++c[typeID];
    //   }
    //   dts[index] = dt;
    //   for (size_t i = 0; i < ntype; ++i) {
    //     double *ptr = res.data() + 5 * (i * dts.size() + index);
    //     *(ptr++) += dx_mean[i];
    //     *(ptr++) += dy_mean[i];
    //     *(ptr++) += dx_squared[i];
    //     *ptr += dy_squared[i];
    //     counts[i * dts.size() + index] += c[i];
    //   }
    // }
  }

  void cleanup() override {
    auto c = counts.begin();
    auto r = res.begin();
    while (c != counts.end()) {
      if (*c) {        // counts > 0
        *(r++) /= *c;  // dx_mean
        *(r++) /= *c;  // dy_mean
        *(r++) /= *c;  // dz_mean
        // Note: the mean is not subtracted from the square,
        //  i.e., we implicitly assume the true mean is zero
        *(r++) /= *c;      // dx_squared;
        *(r++) /= *c;      // dy_squared;
        *(r++) /= *(c++);  // dz_squared;
        *r = *(r - 1) + *(r - 2) + *(r - 3);
        ++r;
      } else {
        ++c;
        r += 7;
      }
    }
  }
};

// class Fsq_t {
// public:
//   std::vector<size_t> dts;
//   std::vector<size_t> counts;
//   std::vector<double> res;         // data container
//   std::vector<double> x, y, z, w;  //weight and sample points
//   double q;

//   Fsq_t(const size_t n, double q) :
//       Fsq_t(n, n, n, q) {}

//   Fsq_t(const size_t n_dts, const size_t n_counts, const size_t n_res, double q) :
//       dts(n_dts), counts(n_counts), res(n_res), q(q) {}

//   // move constructor
//   // https://stackoverflow.com/questions/35830393/c-move-constructor-for-class-containing-vector
//   Fsq_t(Fsq_t &&dp) = default;
//   Fsq_t(const Fsq_t &dp) = delete;

//   // update the data for each cf1 - cf2
//   void update(const size_t index, const size_t dt, const Disp &disp) {
//     // if (isMonotype) {  // compiler should choose which one
//     dtype ds = 0.0, X = 0;
//     // #pragma parallel for
//     for (size_t i = 0; i < disp.dx.size(); i++) {
//       ds = disp.dx[i] * disp.dx[i];
//       ds += disp.dy[i] * disp.dy[i];
//       ds += disp.dz[i] * disp.dz[i];
//       X = this->q * std::sqrt(ds);

//       res[index] += math::itj0(X) / ds;
//     }

//     dts[index] = dt;
//     counts[index] += disp.dx.size();
//   };
//   // clean up the results, e.g. perform mean/std operation
//   void cleanup() {
//     for (size_t i = 0; i < res.size(); i++) {
//       res[i] /= 2 * counts[i] * q * q;
//     }
//   }

//   void set2D() {
//     // try 50 first
//     int N = 50;
//     const double PI = acos(-1);
//     x.resize(N);
//     y.resize(N);
//     // w.resize(N,1);
//     for (size_t i = 0; i < N; i++) {
//       x[i] = cos(i * 2 * PI / N);
//       y[i] = sin(i * 2 * PI / N);
//     }
//   }

//   void set3D() {
//     // the number must be a prior choosen from LDNS
//     // 6, 14, 26, 38, 50, 74, 86, 110, 146, 170, 194, 230, 266, 302, 350, 434,
//     // 590, 770, 974, 1202, 1454, 1730, 2030, 2354, 2702, 3074, 3470, 3890,
//     // 4334, 4802, 5294, 5810

//     // try 50 first
//     int N = 50;

//     x.resize(N);
//     y.resize(N);
//     z.resize(N);
//     w.resize(N);
//     ld(x.data(), y.data(), z.data(), w.data(), &N);
//   }
// };

// Fsq_t fsqt(const Data &data, double q) {
//   /* compute possible dts in logrithmic scale, equivalent to the following python code:
//            dtindex = np.unique((np.exp(np.log(1.15)*np.arange(300)) + 0.5).astype(np.int))
//            assert dtindex[-1] > maxNframe
//            dtindex = dtindex[dtindex < maxNframe]
//            Note that disp(dt=0)=0, so the result should be calculated analytically */

//   // this is generateed by class Data
//   // assert_frameIDs_evenly_separated();
//   if (data.empty()) {
//     Fsq_t property(0, q);
//     return property;
//   }

//   const size_t deltaF = std::abs(data.fileIndex[1] - data.fileIndex[0]);
//   const size_t nframe = data.size();

//   std::vector<size_t> dts(1);  // the first one is zero
//   size_t dt = 1, k = 1;
//   while (dt < nframe) {
//     if (dt != dts.back())
//       dts.push_back(dt);
//     dt = static_cast<size_t>(std::exp(std::log(1.15) * (k++)) + 0.5);
//   }

//   bool is2d = false;

//   // set up sample points and weight
//   Fsq_t property(dts.size(), q);
//   if (data.front()->is2D()) {
//     property.set2D();
//     is2d = true;
//   } else {
//     property.set3D();
//   }
//   dts.push_back(nframe);  // to make sure the following inner loop stop correctly
//   for (size_t i = 0; i < nframe; ++i) {
//     Frame *fold = data[i];
//     /* The following loop has logical error, since dts[dts.size()] is undefined
//                Error: for (k = 1; i+dts[k]<nframe; ++k) { // dts[k=0]=0
//                However, k<dts.size() is also incorrect.
//                Hence we add the last element of dts to be nframe,
//                to make sure the following loop stop correctly.  */
// #pragma omp parallel for
//     for (k = 1; i + dts[k] < nframe; ++k) {  // dts[k=0]=0
//       Frame *fnew = data[i + dts[k]];
//       //std::cout << '(' << k << ',' << i << ',' << i+dts[k] << ')' << std::endl;
//       property.update(k, dts[k] * deltaF, *fnew - *fold);
//     }
//   }

//   property.cleanup();
//   return property;
// }

#endif /* DISPLACEMENT_H */

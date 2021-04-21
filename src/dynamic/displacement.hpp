#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include "Frame.hpp"

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
  virtual void update(const size_t index, const size_t dt, const std::vector<Disp> &&disp){};
  // clean up the results, e.g. perform mean/std operation
  virtual void cleanup(){};
};

template <bool isMonotype>
class MSD_t : public DispProperty {
public:
  /* vector counts shape = (ntype, ndts)
     * vector res shape = (ntype, ndts, 5),
     * for dx_mean, dy_mean, dx_squared, dy_squared, dx_squared+dy_squared;
     */

  MSD_t(const size_t n, const size_t ntype_) :
      DispProperty(n, ntype_ * n, ntype_ * n * 5, ntype_) {}

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

  void update(const size_t index, const size_t dt, const std::vector<Disp> &&disp) override {
    if (isMonotype) {  // compiler should choose which one
      float dx_mean = 0.0, dy_mean = 0.0, dx_squared = 0.0, dy_squared = 0.0;
      for (const auto &ds : disp) {
        dx_mean += ds.dx;
        dy_mean += ds.dy;
        dx_squared += ds.dx * ds.dx;
        dy_squared += ds.dy * ds.dy;
      }
      //if (index >= dts.size())
      //    throw std::runtime_error("wrong index");
      dts[index] = dt;
      double *ptr = res.data() + 5 * index;
      *(ptr++) += dx_mean;
      *(ptr++) += dy_mean;
      *(ptr++) += dx_squared;
      *ptr += dy_squared;
      counts[index] += disp.size();
    } else {
      std::vector<float> dx_mean(ntype), dy_mean(ntype), dx_squared(ntype), dy_squared(ntype);
      std::vector<size_t> c(ntype);
      for (const auto &ds : disp) {
        const int typeID = typeIDs[ds.pIndex];
        //std::cout << "typeID = " << typeID << " , " << ds.dx << std::endl;
        dx_mean[typeID] += ds.dx;
        dy_mean[typeID] += ds.dy;
        dx_squared[typeID] += ds.dx * ds.dx;
        dy_squared[typeID] += ds.dy * ds.dy;
        ++c[typeID];
      }
      dts[index] = dt;
      for (size_t i = 0; i < ntype; ++i) {
        double *ptr = res.data() + 5 * (i * dts.size() + index);
        *(ptr++) += dx_mean[i];
        *(ptr++) += dy_mean[i];
        *(ptr++) += dx_squared[i];
        *ptr += dy_squared[i];
        counts[i * dts.size() + index] += c[i];
      }
    }
  }

  void cleanup() override {
    auto c = counts.begin();
    auto r = res.begin();
    while (c != counts.end()) {
      if (*c) {        // counts > 0
        *(r++) /= *c;  // dx_mean
        *(r++) /= *c;  // dy_mean
        // Note: the mean is not subtracted from the square,
        //  i.e., we implicitly assume the true mean is zero
        *(r++) /= *c;      // dx_squared;
        *(r++) /= *(c++);  // dx_squared;
        *r = *(r - 1) + *(r - 2);
        ++r;
      } else {
        ++c;
        r += 5;
      }
    }
  }
};

#endif /* DISPLACEMENT_H */

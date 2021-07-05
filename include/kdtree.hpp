#include <iostream>

#include "Frame.hpp"
#include "nanoflann.hpp"
#include "type.hpp"

using namespace nanoflann;

// And this is the "dataset to kd-tree" adaptor class:

struct FrameAdaptor {
  const Frame& obj;  //!< A const ref to the data set origin

  /// The constructor that sets the data set source
  FrameAdaptor(const Frame& obj_) :
      obj(obj_) {}

  /// CRTP helper method
  inline const Frame& derived() const { return obj; }

  // Must return the number of data points
  inline size_t kdtree_get_point_count() const { return derived().particleN; }

  // Returns the dim'th component of the idx'th point in the class:
  // Since this is inlined and the "dim" argument is typically an immediate value, the
  //  "if/else's" are actually solved at compile time.
  inline dtype kdtree_get_pt(const size_t idx, const size_t dim) const {
    if (dim == 0)
      // return derived().x.at(idx);
      return derived().x[idx];
    else if (dim == 1)
      // return derived().y.at(idx);
      return derived().y[idx];
    else
      // return derived().z.at(idx);
      return derived().z[idx];
  }

  // Optional bounding-box computation: return false to default to a standard bbox computation loop.
  //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
  //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
  template <class BBOX>
  bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }

};  // end of FrameAdaptor

typedef KDTreeSingleIndexAdaptor<
    L2_Simple_Adaptor<dtype, FrameAdaptor>,
    FrameAdaptor,
    3>
    FrameKdTree;

/** Squared Euclidean (L2) distance functor (suitable for low-dimensionality
 * datasets, like 2D or 3D point clouds) Corresponding distance traits:
 * nanoflann::metric_L2_Simple \tparam T Type of the elements (e.g. double,
 * float, uint8_t) \tparam _DistanceType Type of distance variables (must be
 * signed) (e.g. float, double, int64_t)
 */
template <class T, class DataSource, typename _DistanceType = T>
struct L2_PBC_Adaptor {
  typedef T ElementType;
  typedef _DistanceType DistanceType;

  const DataSource& data_source;

  const T boxsize[3] = {
      data_source.derived().xl,
      data_source.derived().yl,
      data_source.derived().zl,
  };

  L2_PBC_Adaptor(const DataSource& _data_source) :
      data_source(_data_source) {}

  inline DistanceType evalMetric(const T* a, const size_t b_idx,
                                 size_t size) const {
    DistanceType result = DistanceType();
    for (size_t i = 0; i < size; ++i) {
      const DistanceType diff = periodic_distance(a[i] - data_source.kdtree_get_pt(b_idx, i), boxsize[i]);
      if (diff != (a[i] - data_source.kdtree_get_pt(b_idx, i))) {
        std::cout << "PBC USED in evalMetric, id=" << b_idx << std::endl;
        std::cout << boxsize[0] << boxsize[1] << boxsize[2] << std::endl;
      }
      result += diff * diff;
    }
    return result;
  }

  template <typename U, typename V>
  inline DistanceType accum_dist(const U a, const V b, const size_t idx) const {
    U diff = periodic_distance((a - b), boxsize[idx]);
    if (diff != (a - b)) {
      std::cout << "PBC USED in accum_dist, id=" << idx << std::endl;
      std::cout << boxsize[0] << boxsize[1] << boxsize[2] << std::endl;
    }
    return diff * diff;
  };
};

typedef KDTreeSingleIndexAdaptor<
    L2_PBC_Adaptor<dtype, FrameAdaptor>,
    FrameAdaptor,
    3>
    FrameKdTreePBC;
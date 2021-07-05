#ifndef _COVER_TREE_POINT_H
#define _COVER_TREE_POINT_H

#include <vector>

/**
 * A simple point class containing a vector of doubles and a single char name.
 */
class CoverTreePoint {
private:
  std::vector<double> _vec;
  size_t _id;

public:
  CoverTreePoint(std::vector<double> v, size_t id) :
      _vec(v), _id(id) {}
  double distance(const CoverTreePoint& p) const;
  const std::vector<double>& getVec() const;
  const size_t& getId() const;
  void print() const;
  bool operator==(const CoverTreePoint&) const;
};

#endif  // _COVER_TREE_POINT_H
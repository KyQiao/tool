#ifndef _FRAME_H
#define _FRAME_H

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>
// cover tree not used
// #include "Cover_Tree.hpp"
#include "Table.hpp"
#include "type.hpp"

inline double periodic_distance(double x, double lx) {
  return (std::abs(x) > 0.5 * lx) ? ((x > 0) ? x - lx : x + lx) : x;
};

struct Disp {
  std::vector<dtype> dx, dy, dz;
  long dt;
};

// // point struct for CoverTree class
// struct Point {
//   std::vector<double> _vec;
//   size_t _id;
//   std::vector<dtype>* _boxsize;

//   Point(std::vector<double> v, size_t id, std::vector<dtype>* boxsize) :
//       _vec(v), _id(id), _boxsize(boxsize) {}

//   double distance(const Point& p) const {
//     const std::vector<double>& vec = p.getVec();
//     double dist = 0;
//     size_t lim = vec.size();
//     for (int i = 0; i < lim; i++) {
//       double d = periodic_distance((vec[i] - _vec[i]), _boxsize->operator[](i));
//       dist += d * d;
//     }
//     dist = sqrt(dist);
//     return dist;
//   };
//   const std::vector<double>& getVec() const {
//     return _vec;
//   };
//   const size_t& getId() const {
//     return _id;
//   };
//   // void print() const;
//   bool operator==(const Point& p) const {
//     return (_vec == p.getVec() && _id == p.getId());
//   };
// };

class Frame {
public:
  unsigned long timestep = 0;
  unsigned long particleN = 0;
  // by default only one type is used
  size_t ntype = 1;
  dtype boxXL = 0;
  dtype boxXH = 0;
  dtype boxYL = 0;
  dtype boxYH = 0;
  dtype boxZL = 0;
  dtype boxZH = 0;
  dtype xl = 0, yl = 0, zl = 0;
  std::vector<unsigned long> id{};
  std::vector<unsigned long> type{};
  std::vector<dtype> x{};
  std::vector<dtype> y{};
  std::vector<dtype> z{};
  std::vector<dtype> xu{};
  std::vector<dtype> yu{};
  std::vector<dtype> zu{};
  // not used
  // CoverTree<Point> cTree;
  Table attr_table;        //where the rest properties store [property][particle ID]
  std::string attr_order;  //i.e. "q c_pe c_voro[1] c_ackland ... "

  std::unordered_map<std::string, size_t> attr_index;  //dict store index and names. i.e. d["q"]=0 d["c_pe"]=1
  Frame(){};
  Frame(std::string file) {
    read(file);
  }
  ~Frame(){};

  enum compressType
  {
    none,
    gz,
    zstd
  };

  // file io port
  // when loading huge file. Mmap might be used.
  // https://www.reddit.com/r/cpp/comments/318m4n/how_to_read_a_huge_file_fast/
  // google loading file faster c++
  // consider dumping hdf5 file
  void readLammps(const std::string& fileName, compressType compress = none, bool readAttr = true);

  // general io port
  void read(const std::string& fileName) {
    // default with format of LAMMPS dump file
    if (fileName.substr(fileName.size() - 2) == "gz")
      readLammps(fileName, compressType::gz);
    else
      readLammps(fileName, compressType::none);
  }
  template <typename fileInput>
  void readLammpsBase(fileInput&, bool readAttr);

  // select particle in place. will change all the data
  void select(std::string);

  // Frame& select(std::string);

  void describe();

  bool isInBox();

  bool is2D() const;

  bool is3D() const;

  bool is2D();

  bool is3D();

  std::vector<dtype>& get(std::string);

  // the defination of displacement data follows:
  // the first is new, the second is old
  // whether sort or not, the disp vector will have ordered id.
  // id from 1 to N
  Disp operator-(const Frame& fnew) {
    Disp disp;
    disp.dt = fnew.timestep - this->timestep;
    disp.dx.resize(this->particleN);
    disp.dy.resize(this->particleN);
    for (size_t i = 0; i < this->particleN; i++) {
      // loop for id i
      disp.dx[this->id[i] - 1] -= this->xu[this->id[i] - 1];
      disp.dx[fnew.id[i] - 1] += fnew.xu[fnew.id[i] - 1];
      disp.dy[this->id[i] - 1] -= this->yu[this->id[i] - 1];
      disp.dy[fnew.id[i] - 1] += fnew.yu[fnew.id[i] - 1];
    }
    if (is3D()) {
      for (size_t i = 0; i < this->particleN; i++) {
        disp.dz.resize(this->particleN);
        disp.dz[this->id[i] - 1] -= this->zu[this->id[i] - 1];
        disp.dz[fnew.id[i] - 1] += fnew.zu[fnew.id[i] - 1];
      }
    }
    return std::move(disp);
  };

  // Point data2Point(size_t i) {
  //   std::vector<dtype> boxsize = {xl, yl, zl};
  //   auto* b = &boxsize;
  //   return Point({x[i], y[i], z[i]}, id[i], b);
  // };

  // void buildCoverTree() {
  //   std::vector<dtype> boxsize = {xl, yl, zl};
  //   auto* b = &boxsize;

  //   for (size_t i = 0; i < particleN; i++) {
  //     cTree.insert(Point({x[i], y[i], z[i]}, id[i], b));
  //   }
  //   if (cTree.isValidTree())
  //     std::cout << "Insert test: \t\t\t\tPassed\n";
  //   else
  //     std::cout << "Insert test: \t\t\t\tFailed\n";
  // };
};

#endif
#ifndef _FRAME_H
#define _FRAME_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Table.hpp"

class Frame {
public:
  unsigned long timestep = 0;
  unsigned long particleN = 0;
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

  Table attr_table;
  std::string attr_order; 

  std::unordered_map<std::string,size_t> attr_index;
  Frame(){};
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
  void readLammps(const std::string& fileName, compressType compress = none,bool readAttr=true);

  template <typename fileInput>
  void readLammpsBase(fileInput&,bool readAttr);

// select particle in place. will change all the data
  void select(std::string);

  // Frame& select(std::string);

  void describe();

  bool isInBox();

  bool is2D() const;

  bool is3D() const;
};

#endif
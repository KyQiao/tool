#ifndef _DATA_H
#define _DATA_H

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L)) && defined(__has_include)
#if __has_include(<filesystem>) && (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include "filesystem.hpp"
namespace fs = ghc::filesystem;
#endif

#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Frame.hpp"
#include "Table.hpp"
size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

class Data {
public:
  std::vector<size_t> fileIndex;
  std::string pattern;
  std::string folder;
  std::vector<std::unique_ptr<Frame>> data;
  Data(){};
  Data(std::string path) {
    readFolder(path);
  }
  Data(std::string path, std::string pattern) {
    readFolder(path, pattern);
  }
  ~Data(){

  };

  // directly read a folder and load index to
  void readFolder(std::string path, std::string pattern = "atom.*.xyz");
  //load data
  // low and high are the index of fileIndex vector.
  void load(size_t step = 1, size_t low = 0, size_t high = SIZE_T_MAX);
};

#endif
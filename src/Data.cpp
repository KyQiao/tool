#include "Data.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "omp.h"

// #include "make_unique.hpp"

#define Debug 0

size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

std::string
get_stem(const fs::path &p) {
  return (p.stem().string());
}

std::string get_filename(const fs::path &p) {
  return (p.filename().string());
}

size_t string2size_t(const std::string &numStr) {
  // int size = numStr.size();
  size_t ans = 0;
  // std::cout << numStr << std::endl;
  for (const char &s : numStr)
    if ((s <= 57) && (s >= 48))
      ans = ans * 10 + (s - 48);
    else {
#if Debug
      std::cout << "string convert fail" << std::endl;
#endif
      return SIZE_T_MAX;
    }
  return ans;
}

size_t match(const std::string &s, const std::string &pattern) {
  // match pattern should be:
  // some_words.*.some_words
  // * is the number of steps

  // algorithm
  // 0123456789
  // atom.*.xyz
  // size=10 find result=5
  // left=5 right=4
  // for input string
  // 012345678901
  // atom.100.xyz
  // size=12
  // s.substr(0,5)=atom.
  // s.substr(size-right=8)=.xyz
  // pattern.substr(n+1,right)=(6,4)=.xyz
  // result s.substr(5,12-5-4+1=4)=100

  // test case
  // some_words.*.some_words
  // some_words.*
  // *.some_words
  std::string::size_type n;
  n = pattern.find('*');
  // size of left string and right string
  int left = n, right = pattern.size() - n - 1;
  int size = s.size();

#if Debug
  std::cout << "input string is " << s
            << " and pattern is " << pattern << std::endl;
#endif
  if (s.substr(0, left) == pattern.substr(0, left))
    if (s.substr(size - right) == pattern.substr(n + 1))
      return string2size_t(s.substr(left, size - right - left));
      // return std::stoul(s.substr(left, size - right-left));
#if Debug
  std::cout << "match fail" << std::endl;
#endif
  return SIZE_T_MAX;
}

void outputStringList(const std::vector<std::string> &fileList) {
  for (const auto &path : fileList) {
    std::cout << path << std::endl;
  }
}

void Data::readFolder(std::string path, std::string pattern) {
  fs::path dirName = fs::u8path(path);
  this->folder = dirName.string() + "/";
#if Debug
  // for output filelist
  std::vector<std::string> fileList;
  uint64_t totalSize = 0;
  int totalFiles = 0;
#endif
  std::string fileName;
  this->pattern = pattern;

  try {
    auto rdi = fs::recursive_directory_iterator(dirName);
    for (const auto &de : rdi) {
      // de for directory entry
      // only look for file with depth 0(current folder)
      if (rdi.depth() == 0) {
        if (de.is_regular_file()) {
          fileName = get_filename(de.path());
          size_t step = match(fileName, pattern);
          if (step != SIZE_T_MAX) {
            this->fileIndex.push_back(step);
#if Debug
            totalSize += de.file_size();
            fileList.push_back(fileName);
            std::cout << "extract step number is " << step
                      << " and the filename is " << fileName << std::endl;
            ++totalFiles;
#endif
          }
        }
      }
    }
  } catch (fs::filesystem_error fe) {
    std::cerr << "Error: " << fe.what() << std::endl;
    exit(1);
  }
#if Debug
  std::cout << totalSize / 1024.0 / 1024.0 << " Mbytes in " << totalFiles << " files" << std::endl;
  outputStringList(fileList);
#endif
  // non-desending order
  std::sort(this->fileIndex.begin(), this->fileIndex.end());
}

void Data::load(size_t step, size_t low, size_t high) {
  std::string::size_type n;
  n = this->pattern.find('*');
  std::string left = pattern.substr(0, n), right = pattern.substr(n + 1);
  size_t size = this->fileIndex.size();
#if Debug
  std::cout << "left part is " << left
            << " and right part is " << right << std::endl;
#endif
  // make sure inputs are in the range
  if (high > size)
    high = size;
  if (high < low)
    throw std::runtime_error("current range contains no file");

  this->data.reserve((high - low) / step);


// #pragma parallel for
// parallel io for openmp cannot apply here
  for (size_t i = low; i < high; i += step) {
#if Debug
    std::cout << "reading file:" << this->folder + left + std::to_string(fileIndex[i]) + right << std::endl;
#endif
    this->data.push_back(new Frame());
    this->data.back()->read(this->folder + left + std::to_string(fileIndex[i]) + right);
  }

  // while (low < high) {
  //   this->data.push_back(new Frame());
  //   this->data.back()->read(this->folder + left + std::to_string(fileIndex[low]) + right);
  //   low += step;
  // }
  std::cout << "reading " << data.size() << " files." << std::endl;
}
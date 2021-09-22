#ifndef _TABLE_H
#define _TABLE_H

#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "type.hpp"

class Table {
private:
  std::vector<std::vector<dtype>> data;

public:
  Table(){};

  // using a list of vector to init class
  Table(std::initializer_list<std::vector<dtype>> l) :
      data(l) {}
  ~Table(){};

  void output(std::string filename) {
    std::fstream outfile;
    // change to app then you can customize header
    outfile.open(filename, std::ios::out | std::ios::app);

    for (size_t j = 0; j < this->rows(); j++) {
      for (size_t i = 0; i < this->cols(); i++) {
        outfile << this->data[i][j] << " ";
      }
      outfile << "\n";
    }

    outfile.close();

    std::cout << "writing complete" << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& out, const Table& t) {
    for (size_t j = 0; j < t.rows(); j++) {
      for (size_t i = 0; i < t.cols(); i++) {
        out << t.data[i][j] << " ";
      }
      out << "\n";
    }
    return out;
  };

  size_t cols() const {
    return data.size();
  }
  size_t rows() const {
    if (data.empty())
      return 0;
    else
      return data.at(0).size();
  }

  // dangerous functions
  // do not shrink the data.
  void setcols(size_t i) {
    while (this->data.size() < i)
      this->data.push_back(std::vector<dtype>(rows()));
  }

  void setrows(size_t i) {
    for (auto& v : this->data)
      v.resize(i);
  }

  std::vector<dtype>& operator[](size_t idx) {
    return this->data.operator[](idx);
  }

  // expose vector iterator
  std::vector<std::vector<dtype>>::iterator begin() {
    return data.begin();
  }

  std::vector<std::vector<dtype>>::iterator end() {
    return data.end();
  }

  std::vector<std::vector<dtype>>::const_iterator cbegin() const {
    return data.cbegin();
  }

  std::vector<std::vector<dtype>>::const_iterator cend() const {
    return data.cend();
  }

  void push_back(std::vector<double>& v) {
    data.push_back(v);
  }
};

// void Table::output(std::string filename) {
//   std::fstream outfile;
//   outfile.open(filename, std::ios::out);

//     for (size_t j = 0; j < this->rows() - 1; j++) {
//       for (size_t i = 0; i < this->cols() - 2; i++) {
//         outfile << this->data[i][j] << " ";
//       }
//       outfile << this->data[this->cols()][j] << "\n";
//     }

//   outfile.close();

//   std::cout << "writing complete" << std::endl;
// }

#endif
#include <omp.h>
#include <time.h>

#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>

#include "Frame.hpp"
#include "Table.hpp"
#include "vptree.hpp"

#define debug 0

using point_t = std::vector<double>;

std::vector<point_t> generateData(size_t size, int dim = 3) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 20);
  std::vector<point_t> v(size, point_t(3));
  for (size_t i = 0; i < size; i++)
    for (int j = 0; j < dim; j++)
      v[i][j] = dis(gen);

  return v;
}

std::vector<double> generateDiameter(size_t size) {
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::normal_distribution<> d{1, 0.1};
  std::vector<double> v(size);
  for (size_t i = 0; i < size; i++)
    v[i] = d(gen);
  return v;
}

inline double Sigma_ij(double sigma_i, double sigma_j) {
  return (sigma_i + sigma_j) / 2.0 * (1 - 0.2 * std::abs(sigma_i - sigma_j));
}

// using LJ unit
double PolyLJ12(const double& dist, const double& sigma_ij, double cutoff = 2.5) {
  double ratio = sigma_ij / dist;
  if (ratio > 2.5 || dist == 0)
    return 0;
  double r2 = ratio * ratio;
  double r6 = r2 * r2 * r2;
  double r12 = r6 * r6;
  double cutoff2 = cutoff * cutoff;
  double cutoff6 = cutoff2 * cutoff2 * cutoff2;
  double cutoff12 = cutoff6 * cutoff6;
  return 4 * (r12 - r6) - 4 * (cutoff12 - cutoff6);
}

double PolyLJ12_25(const double& dist, const double& sigma_ij) {
  double ratio = sigma_ij / dist;
  if (ratio > 2.5 || dist == 0)
    return 0;
  double r2 = ratio * ratio;
  double r6 = r2 * r2 * r2;
  double r12 = r6 * r6;
  const double cutoff2 = 2.5 * 2.5;
  const double cutoff6 = cutoff2 * cutoff2 * cutoff2;
  const double cutoff12 = cutoff6 * cutoff6;
  return 4 * (r12 - r6) - 4 * (cutoff12 - cutoff6);
}

void swap(const std::vector<point_t>& data, std::vector<double>& diameter, size_t step, double beta, std::vector<double> dimensions, std::vector<bool> periodicity) {
  double cutoff_radius = 2;

  auto metric = vpt::EuclideanMetricPbc<point_t>(dimensions, periodicity);
  auto tree = vpt::VpTree(data, metric);

  auto size = data.size();
  auto maxD = *std::max_element(diameter.begin(), diameter.end());

  // lock for vector
  omp_lock_t* index_locks = new omp_lock_t[size];
  // lock for using the lock
  omp_lock_t usingLock;

  size_t particleI, particleJ;

  // set lock
#pragma omp parallel for
  for (size_t i = 0; i < size; i++) {
    omp_init_lock(&index_locks[i]);
  }
  omp_init_lock(&usingLock);

  double sigma_i, sigma_j, sigma_ij;

#pragma omp parallel for private(particleI, particleJ, sigma_i, sigma_j, sigma_ij)
  // #pragma omp parallel for
  for (size_t ind = 0; ind < step; ind++) {
    // random index choose

    // a thread safe random generator
    // https://stackoverflow.com/questions/21237905/how-do-i-generate-thread-safe-uniform-random-numbers
#if defined(_MSC_VER)  // Visual studio
#define thread_local __declspec(thread)
#elif defined(__GCC__)  // GCC
#define thread_local __thread
#endif
    static thread_local std::mt19937 gen(clock() + omp_get_thread_num());
    std::uniform_int_distribution<int> dis(0, size - 1);
    std::uniform_real_distribution<double> acc(0, 1);
    // choose particle
    particleI = dis(gen);
    particleJ = dis(gen);
    while (particleJ == particleI)
      particleJ = dis(gen);

    sigma_i = diameter[particleI];
    sigma_j = diameter[particleJ];
    // sigma_ij = Sigma_ij(sigma_i, sigma_j);

    double r = std::max(sigma_i, sigma_j);

    // get neighbor index

    auto NeighborI = tree.getNeighborhoodPoints(data[particleI], 2.5 * Sigma_ij(r, maxD));
    auto NeighborJ = tree.getNeighborhoodPoints(data[particleJ], 2.5 * Sigma_ij(r, maxD));

    double E_move_I = 0, E_put_I = 0, E_move_J = 0, E_put_J = 0;

    // set the lock
    std::unordered_set<size_t> particleInvolve;
    for (size_t i = 0; i < NeighborI.first.size(); i++)
      particleInvolve.insert(NeighborI.second[i]);
    for (size_t i = 0; i < NeighborJ.first.size(); i++)
      particleInvolve.insert(NeighborJ.second[i]);

    // omp safe lock
    omp_set_lock(&usingLock);
    for (const size_t& v : particleInvolve)
      omp_set_lock(&index_locks[v]);
    omp_unset_lock(&usingLock);

    // calculate energy change
    for (size_t i = 0; i < NeighborI.first.size(); i++) {
      E_move_I += PolyLJ12_25(NeighborI.first[i], Sigma_ij(sigma_i, diameter[NeighborI.second[i]]));
      E_put_J += PolyLJ12_25(NeighborI.first[i], Sigma_ij(sigma_j, diameter[NeighborI.second[i]]));
    }
    for (size_t i = 0; i < NeighborJ.first.size(); i++) {
      E_put_I += PolyLJ12_25(NeighborJ.first[i], Sigma_ij(sigma_i, diameter[NeighborJ.second[i]]));
      E_move_J += PolyLJ12_25(NeighborJ.first[i], Sigma_ij(sigma_j, diameter[NeighborJ.second[i]]));
    }

    double DelatE = E_put_I - E_move_I + E_put_J - E_move_J;

    // swap
    if ((DelatE < 0) || (std::exp(-beta * DelatE) > acc(gen))) {
      diameter[particleJ] = sigma_i;
      diameter[particleI] = sigma_j;
    }

    // unlock
    for (const size_t& v : particleInvolve)
      omp_unset_lock(&index_locks[v]);
    particleInvolve.clear();

#if debug
    {
#pragma omp critical
      std::cout << particleI << " " << particleJ << "from thread " << omp_get_thread_num() << std::endl;
      std::cout << "get " << NeighborI.first.size() - 1 << " neighbors of I" << std::endl;
      std::cout << "get " << NeighborJ.first.size() - 1 << " neighbors of J" << std::endl;
      std::cout << "Energy change is: " << DelatE << "\n"
                << std::endl;
    }

#endif
  }  //end of loop

  std::cout << "Swap over" << std::endl;

  // release all lock
#pragma omp parallel for
  for (size_t i = 0; i < size; i++)
    omp_destroy_lock(&index_locks[i]);
  delete[] index_locks;
}

int main(int argc, char const* argv[]) {
  // std::vector<bool> periodicity = {true, true, true};
  // std::vector<double> dimensions = {20, 20, 20};

  // auto data = generateData(1000, 3);
  // auto diameter = generateDiameter(1000);
  // std::cout << "data generated" << std::endl;

  std::string filename = argv[1];
  auto data = Frame();
  std::cout << "reading file: " << filename << std::endl;
  data.read(filename);
  size_t step = std::stoi(argv[2]);

  // Table points{data.x, data.y, data.z};
  using point_t = std::vector<double>;
  std::vector<point_t> points;
  for (size_t i = 0; i < data.particleN; i++) {
    points.push_back({data.x[i], data.y[i], data.z[i]});
  }

  auto diameter = generateDiameter(points.size());

  auto out1 = diameter;

  std::vector<bool> periodicity = {true, true, true};
  std::vector<double> dimensions = {data.xl, data.yl, data.zl};
  swap(points, diameter, step, 1, dimensions, periodicity);

  double change;
  for (size_t i = 0; i < diameter.size(); i++) {
    change += std::abs(diameter[i] - out1[i]);
  }

  std::cout << change << std::endl;

  return 0;
}

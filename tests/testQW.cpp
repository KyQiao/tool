#include "Frame.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "voro++/voro++.hh"
// #include "libqhullcpp/Qhull.h"
#include <cmath>

using namespace voro;

// void voroTest(const Frame &data)
// {
//     double lx = data.boxXH - data.boxXL;
//     double ly = data.boxYH - data.boxYL;
//     double lz = data.boxXH - data.boxXL;
//     double tvol = lx * ly * lz;
//     double Cn = std::cbrt(data.particleN / 8 / lx / ly / lz);
//     std::cout << Cn << std::endl;
//     voro::container con(data.boxXL, data.boxXH, data.boxYL, data.boxYH, data.boxZL, data.boxZH,
//                         int(std::ceil(Cn * lx)), int(std::ceil(Cn * ly)), int(std::ceil(Cn * lz)),
//                         true, true, true, 8);

//     std::cout << "Con start" << std::endl;
//     // int errcnt = 0;
//     for (size_t i = 0; i < data.particleN; i += 100)
//         con.put(data.id[i], data.x[i], data.y[i], data.z[i]);

//     std::cout << "Con completed" << std::endl;
//     double vvol = con.sum_cell_volumes();
//     std::cout << "Container volume :" << tvol << std::endl
//               << "Voronoi volume   : " << vvol << std::endl
//               << "Difference       : " << vvol - tvol << std::endl;
// }

int main(int argc, char const *argv[])
{
    Frame data = Frame();
    // data.readXYZgz("/mnt/d/data/Simulation/data/atom.10000000.xyz.gz");
    // data.readXYZgz("/mnt/d/data/Simulation/data/atom.9000.xyz.gz");
    data.readXYZgz(argv[1]);

    // data.describe();
    // auto result = std::min_element(std::begin(data.id), std::end(data.id));
    // std::cout << "min element at: " << std::distance(std::begin(data.id), result)<<"\n";
    // std::cout << "min element at: " << data.id[std::distance(std::begin(data.id), result)];

    // if (data.isInBox())
    //     std::cout << "xyz is in box\n";
    // else
    //     std::cout << "xyz is not in box\n

    data.ql();
    std::ofstream myfile;
    // auto file = std::string(argv[1]);
    // auto file = std::string("/mnt/d/data/Simulation/data/atom.9000.xyz.gz");
    myfile.open(argv[2]);
    myfile << "q6 "
           << "q4 "
           << "w6 "
           << "w4 "
           << "\n";
    double q6mean = 0, q4mean = 0, w6mean = 0, w4mean = 0;
    for (size_t i = 0; i < data.q6.size(); i++)
    {
        q6mean += data.q6[i];
        q4mean += data.q4[i];
        w6mean += data.w6[i];
        w4mean += data.w4[i];
        myfile << data.q6[i] << " " << data.q4[i] << " " << data.w6[i] << " " << data.w4[i] << "\n";
    }
    myfile.close();

    std::cout << "avg q6 is : " << q6mean / data.particleN << "\n";
    std::cout << "avg q4 is : " << q4mean / data.particleN << "\n";
    std::cout << "avg w6 is : " << w6mean / data.particleN << "\n";
    std::cout << "avg w4 is : " << w4mean / data.particleN << "\n";
    return 0;
}

// g++ -Wall Frame.cpp Frame.hpp test.cpp -lvoro++ -lz -O3 -o test.o
// g++ -Wall Frame.cpp Frame.hpp test.cpp libsh.so -lvoro++ -lz -O3 -o test.o

// g++ -Wall --std=c++11 Frame.cpp Frame.hpp test.cpp libsh.so -lvoro++ -lz -O3 -o q6_correct
// g++ -Wall --std=c++11 Frame.cpp Frame.hpp test.cpp libsh.so -lvoro++ -lz -O3 -o qw

// clang++ -fsanitize=memory  -fsanitize=address  -Wall --std=c++11 -g Frame.cpp Frame.hpp test.cpp libsh.so -lvoro++ -lz -o qw_clang
// clang++  -fsanitize=address  -Wall --std=c++11 -g Frame.cpp Frame.hpp test.cpp libsh.so -lvoro++ -lz 
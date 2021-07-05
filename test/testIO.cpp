#include <iostream>
#include <string>
#include "../Frame.hpp"
#include <memory>

void testFrame()
{
    std::string path = "../../data/";
    std::string file = "atom.10000000.xyz.gz";
    std::string fileName = path + file;
    // std::string fileName = "./test.txt.gz";
    std::unique_ptr<Frame> data = readXYZgz(fileName);
    std::cout << "loading complete" << std::endl;
    std::cout << data->x.size() << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << data->x[i] << " " << data->y[i] << " " << data->z[i] << " " << data->c_pe[i] << " "
                  << data->c_voro[i] << " " << data->id[i] << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    testFrame();
    return 0;
}

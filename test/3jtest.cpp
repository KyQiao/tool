#include <iostream>
#include <iomanip>
#include <string>
#include "wigner3j.hpp"

int main(int argc, char const *argv[])
{
    int a = std::stoi(argv[1]);
    int b = std::stoi(argv[2]);

    // std::cout << "6,6,6"<< "\n";
    // std::cout << a << "," << b << "," << -a - b << " is " << std::setprecision(20) << w3_6(a, b)<<"\n";
    std::cout << "4,4,4\n";
    std::cout << a << "," << b << "," << -a - b << " is " << std::setprecision(20) << w3_4(a, b) << "\n";

    return 0;
}
#include <iostream>
#include <string>
#include <cmath>

// Fast mod for double type
double FastFMod(double x, double m)
{
    return x - (m * std::floor(x / m));
}

int main(int argc, char const *argv[])
{
    std::cout << FastFMod(std::stod(argv[1]), std::stod(argv[2])) << "\n";
    return 0;
}

// g++ -O3 fmod.cpp -o fmod
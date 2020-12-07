#ifndef STRUCTUREFACTOR_H
#define STRUCTUREFACTOR_H

#include "Base.hpp"

Table sk2d(const Frame& data, size_t window, double kmax);

Table sk3d(const Frame& data, size_t window, double kmax);

Table sk3d_fft(const Frame& data, size_t window, double kmax);


#endif
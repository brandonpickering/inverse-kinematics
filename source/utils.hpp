#ifndef _UTILS_HPP
#define _UTILS_HPP


#include <cstdio>
#include <string>


template <typename... T>
std::string stringf(const char *format, T... args) {
  size_t size = snprintf(nullptr, 0, format, args...);
  char *data = new char[size + 1];
  snprintf(data, size + 1, format, args...);
  std::string result(data);
  delete[] data;
  return result;
}


#define PI 3.14159265358979

template <typename T>
T angle_rep(T angle) {
  while (angle >= PI) angle -= 2*PI;
  while (angle < -PI) angle += 2*PI;
  return angle;
}


#endif

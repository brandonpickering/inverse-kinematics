#ifndef _VECTOR_INL
#define _VECTOR_INL


#include <string>

#include "utils.hpp"


template <typename T, size_t N>
struct vector {
  T data[N];
};

template <typename T>
struct vector<T, 2> {
  union {
    T data[2];
    struct { T x, y; };
  };
};

template <typename T>
struct vector<T, 3> {
  union {
    T data[3];
    struct { T x, y, z; };
  };
};

template <typename T>
struct vector<T, 4> {
  union {
    T data[4];
    struct { T x, y, z, w; };
  };
};


template <size_t N>
using vectorf = vector<lfloat, N>;

typedef vectorf<2> vector2f;
typedef vectorf<3> vector3f;
typedef vectorf<4> vector4f;


template <typename T, size_t N>
std::string str(const vector<T, N> &v) {
  std::string res = "(";
  for (size_t i = 0; i < N; i++) {
    res += stringf("%f", (float) v.data[i]);
    if (i != N - 1)
      res += ", ";
  }
  return res + ")";
}

template <typename T, size_t N>
T square_dist(const vector<T, N> &v1, const vector<T, N> &v2) {
  T result = 0;
  for (size_t i = 0; i < N; i++)
    result += (v1.data[i] - v2.data[i])*(v1.data[i] - v2.data[i]);
  return result;
}

template <typename T, size_t N>
T square_mag(const vector<T, N> &v) {
  T result = 0;
  for (size_t i = 0; i < N; i++)
    result += v.data[i] * v.data[i];
  return result;
}

template <typename T, size_t N>
void comp_clamp_scale(vector<T, N> &dest, const vector<T, N> &v, T clamp) {
  if (N == 0) return;
  T maxc = std::abs(v.data[0]);
  for (size_t i = 1; i < N; i++)
    if (std::abs(v.data[i]) > maxc) maxc = std::abs(v.data[i]);
  if (maxc <= clamp) return;

  for (size_t i = 0; i < N; i++)
    dest.data[i] = v.data[i] * clamp / maxc;
}

template <typename T, size_t N>
void add(vector<T, N> &dest, const vector<T, N> &v1, const vector<T, N> &v2) {
  for (size_t i = 0; i < N; i++)
    dest.data[i] = v1.data[i] + v2.data[i];
}

template <typename T, size_t N>
void sub(vector<T, N> &dest, const vector<T, N> &v1, const vector<T, N> &v2) {
  for (size_t i = 0; i < N; i++)
    dest.data[i] = v1.data[i] - v2.data[i];
}

template <typename T, size_t N>
void mul(vector<T, N> &dest, const vector<T, N> &v, T s) {
  for (size_t i = 0; i < N; i++)
    dest.data[i] = v.data[i] * s;
}


#endif

#ifndef _MATRIX_INL
#define _MATRIX_INL


#include <cassert>
#include <string>

#include "utils.hpp"


template <typename T, size_t M, size_t N>
struct matrix {
  T data[M*N];
};

template <size_t M, size_t N>
using matrixf = matrix<lfloat, M, N>;

typedef matrixf<2, 2> matrix2f;
typedef matrixf<3, 3> matrix3f;
typedef matrixf<4, 4> matrix4f;


template <typename T, size_t M, size_t N>
std::string str(const matrix<T, M, N> &m) {
  std::string res = "";
  for (size_t i = 0; i < M; i++) {
    res += "( ";
    for (size_t j = 0; j < N; j++)
      res += stringf("%f ", m.data[N*i + j]);
    res += ")\n";
  }
  return res;
}

template <typename T, size_t N>
void identity(matrix<T, N, N> &dest) {
  for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < N; j++)
      dest.data[i] = i == j ? 1 : 0;
}

template <typename T, size_t N>
matrix<T, 1, N> &row_vector(vector<T, N> &v) {
  return *(matrix<T, 1, N> *) &v;
}

template <typename T, size_t N>
const matrix<T, 1, N> &row_vector(const vector<T, N> &v) {
  return *(const matrix<T, 1, N> *) &v;
}

template <typename T, size_t M, size_t N>
vector<T, N> &get_row(matrix<T, M, N> &m, size_t index) {
  return *(vector<T, N> *) &m.data[N*index];
}

template <typename T, size_t M, size_t N>
const vector<T, N> &get_row(const matrix<T, M, N> &m, size_t index) {
  return *(const vector<T, N> *) &m.data[N*index];
}

template <typename T, size_t M, size_t N>
void get_row(vector<T, N> &dest, matrix<T, M, N> &m, size_t index) {
  assert(index < M);
  dest = *(vector<T, N> *) &m.data[N*index];
}

template <typename T, size_t M, size_t N>
void set_row(matrix<T, M, N> &m, size_t index, const vector<T, N> &v) {
  assert(index < M);
  *(vector<T, N> *) &m.data[N*index] = v;
}

template <typename T, size_t M, size_t N>
void get_col(vector<T, M> &dest, matrix<T, M, N> &m, size_t index) {
  assert(index < N);
  for (size_t i = 0; i < M; i++)
    dest.data[i] = m.data[N*i + index];
}

template <typename T, size_t M, size_t N>
void set_col(matrix<T, M, N> &m, size_t index, const vector<T, M> &v) {
  assert(index < N);
  for (size_t i = 0; i < M; i++)
    m.data[N*i + index] = v.data[i];
}

template <typename T, size_t M, size_t N>
void transpose(matrix<T, N, M> &dest, const matrix<T, M, N> &m) {
  assert((void *) &dest != (void *) &m);

  for (size_t i = 0; i < M; i++)
    for (size_t j = 0; j < N; j++)
      dest.data[M*j + i] = m.data[N*i + j];
}

template <typename T, size_t M, size_t N>
void add(matrix<T, M, N> &dest, const matrix<T, M, N> &m1,
    const matrix<T, M, N> &m2) {
  for (size_t i = 0; i < M*N; i++)
    dest.data[i] = m1.data[i] + m2.data[i];
}

template <typename T, size_t M, size_t N>
void mul(matrix<T, M, N> &dest, const matrix<T, M, N> &m, T s) {
  for (size_t i = 0; i < M*N; i++)
    dest.data[i] = m.data[i] * s;
}

template <typename T, size_t M, size_t N, size_t O>
void mul(matrix<T, M, O> &dest, const matrix<T, M, N> &m1,
    const matrix<T, N, O> &m2) {
  assert((void *) &dest != (void *) &m1);
  assert((void *) &dest != (void *) &m2);

  for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < O; j++) {
      dest.data[O*i + j] = 0;
      for (size_t k = 0; k < N; k++)
        dest.data[O*i + j] += m1.data[N*i + k] * m2.data[O*k + j];
    }
  }
}

template <typename T, size_t M, size_t N>
void mul(vector<T, M> &dest, const matrix<T, M, N> &m, const vector<T, N> &v) {
  assert((void *) &dest != (void *) &v);

  for (size_t i = 0; i < M; i++) {
    dest.data[i] = 0;
    for (size_t j = 0; j < N; j++)
      dest.data[i] += m.data[N*i + j] * v.data[j];
  }
}


#endif

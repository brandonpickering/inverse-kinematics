#ifndef _LINALG_HPP
#define _LINALG_HPP


typedef double lfloat;

#include "vector.inl"
#include "matrix.inl"


template <size_t M, size_t N>
void row_reduce(matrixf<M, N> &m) {
  size_t row = 0;
  for (size_t col = 0; col < N; col++) {

    // Find row with nonzero first entry
    size_t targ_row;
    lfloat entry;
    for (targ_row = row; targ_row < M; targ_row++)
      if (std::abs(entry = m.data[targ_row*N + col]) >= 0.00001)
        break;
    if (targ_row == M) continue;

    // Place row into top slot, divide by first entry
    for (size_t j = 0; j < N; j++) {
      lfloat temp;
      temp = m.data[targ_row*N + j];
      m.data[targ_row*N + j] = m.data[row*N + j] / entry;
      m.data[row*N + j] = temp / entry;
    }

    // Eliminate rest of column
    for (size_t i = row+1; i < M; i++) {
      entry = m.data[i*N + col];
      for (size_t j = col; j < N; j++)
        m.data[i*N + j] -= m.data[row*N + j] * entry;
    }

    row++;
  }
}


template <size_t M, size_t N>
void lin_solve(vectorf<N> &result, const matrixf<M, N> &m,
    const vectorf<M> &b) {
  static matrixf<M, N+1> aug;
  for (size_t i = 0; i < M; i++)
    for (size_t j = 0; j < N; j++)
      aug.data[i*(N+1) + j] = m.data[i*N + j];
  set_col(aug, N, b);

  row_reduce(aug);

  for (size_t j = 0; j < N; j++) result.data[j] = 0;

  for (size_t i = M; i-- > 0;) {
    size_t j;
    for (j = 0; j < N; j++)
      if (std::abs(aug.data[i*(N+1) + j]) >= 0.00001)
        break;
    if (j == N) continue;

    result.data[j] = aug.data[i*(N+1) + N];

    for (size_t k = j+1; k < N; k++)
      result.data[j] -= aug.data[i*(N+1) + k] * result.data[k];
  }
}


#endif

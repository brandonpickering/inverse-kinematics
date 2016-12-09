#ifndef _ARM3D_HPP
#define _ARM3D_HPP


#include <cmath>

#include "ik_solver.hpp"
#include "linalg.hpp"


template <size_t N>
struct arm3d {
  vector3f root;
  lfloat lengths[N];
};

template <size_t N>
void arm3d_get_trans(matrix3f &result, const arm3d<N> &arm,
    const vectorf<3*N> &state, size_t index) {
  (void) arm;
  assert(index < N);

  vector3f axis = subvec<lfloat, 3*N, 3>(state, 3*index);
  lfloat angle = std::sqrt(square_mag(axis));
  if (angle < 0.000001) // If using deriv sampling, need to adjust this
    identity(result);
  else
    rotate(result, axis, angle);
}

template <size_t N>
void arm3d_get_joint(vector3f &result, const arm3d<N> &arm,
    const vectorf<3*N> &state, size_t index) {
  assert(index <= N);

  result = {0, 0, 0};
  for (size_t i = index; i-- > 0;) {
    result.x += arm.lengths[i];

    matrix3f trans;
    arm3d_get_trans(trans, arm, state, i);
    vector3f temp;
    mul(temp, trans, result);
    result = temp;
  }
  add(result, result, arm.root);
}

template <size_t N>
void arm3d_get_partial(matrix3f &result, const arm3d<N> &arm,
    const vectorf<3*N> &state, size_t index) {
  assert(index < N);

  vector3f end_eff = {0, 0, 0};
  for (size_t i = N-1; i > index; i--) {
    end_eff.x += arm.lengths[i];

    matrix3f trans;
    arm3d_get_trans(trans, arm, state, i);
    vector3f temp;
    mul(temp, trans, end_eff);
    end_eff = temp;
  }
  end_eff.x += arm.lengths[index];

  cross(result, end_eff);
  mul(result, result, (lfloat) -1);

  for (size_t i = index+1; i-- > 0;) {
    matrix3f trans;
    arm3d_get_trans(trans, arm, state, i);
    matrix3f temp;
    mul(temp, trans, result);
    result = temp;
  }
}


template <size_t N>
diff_map<3*N, 3> arm3d_func_samp(const arm3d<N> &arm) {
  diff_map<3*N, 3> map;

  map.value = [arm](vector3f &result, const vectorf<3*N> &state) {
    arm3d_get_joint(result, arm, state, N);
  };

  map.deriv = diff_sample(map.value).deriv;

  return map;
}

template <size_t N>
diff_map<3*N, 3> arm3d_func(const arm3d<N> &arm) {
  diff_map<3*N, 3> map;

  map.value = [arm](vector3f &result, const vectorf<3*N> &state) {
    arm3d_get_joint(result, arm, state, N);
  };

  map.deriv = [arm](matrixf<3, 3*N> &result, const vectorf<3*N> &state) {
    matrix3f partial;
    for (size_t i = 0; i < N; i++) {
      arm3d_get_partial(partial, arm, state, i);
      for (size_t j = 0; j < 3; j++)
        for (size_t k = 0; k < 3; k++)
          result.data[3*N*j + 3*i + k] = partial.data[3*j + k];
    }
  };

  return map;
}


#endif

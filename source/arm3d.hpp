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
void arm3d_get_joint(vector3f &result, const arm3d<N> &arm,
    const vectorf<3*N> &state, size_t index) {
  assert(index <= N);

  result = {0, 0, 0};
  for (size_t i = index; i-- > 0;) {
    result.x += arm.lengths[i];

    vector3f axis = subvec<lfloat, 3*N, 3>(state, 3*i);
    lfloat angle = std::sqrt(square_mag(axis));
    if (angle < 0.00001f) continue;

    matrix3f trans;
    rotate(trans, axis, angle);
    vector3f temp;
    mul(temp, trans, result);
    result = temp;
  }
  add(result, result, arm.root);
}


// arm needs to be persistent
template <size_t N>
diff_map<3*N, 3> arm3d_func(const arm3d<N> &arm) {
  diff_map<3*N, 3> map;

  map.value = [&arm](vector3f &result, const vectorf<3*N> &state) {
    arm3d_get_joint(result, arm, state, N);
  };

  map.deriv = diff_sample(map.value).deriv;

  return map;
}


#endif

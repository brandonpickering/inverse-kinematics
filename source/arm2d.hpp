#ifndef _ARM2D_HPP
#define _ARM2D_HPP


#include "ik_solver.hpp"
#include "linalg.hpp"


template <size_t N>
struct arm2d {
  vector2f root;
  lfloat lengths[N];
};

template <size_t N>
void arm2d_get_joint(vector2f &result, const arm2d<N> &arm,
    const vectorf<N> &state, size_t index) {
  assert(index <= N);

  result = arm.root;
  lfloat transform = 0;
  for (size_t i = 0; i < index; i++) {
    transform += state.data[i];
    result.x += arm.lengths[i] * std::cos(transform);
    result.y += arm.lengths[i] * std::sin(transform);
  }
}

template <size_t N>
void arm2d_get_partial(vector2f &result, const arm2d<N> &arm,
    const vectorf<N> &state, size_t index) {
  assert(index < N);

  vector2f joint;
  arm2d_get_joint(result, arm, state, N);
  arm2d_get_joint(joint, arm, state, index);
  sub(result, result, joint);

  lfloat temp;
  (temp = result.x, result.x = -result.y, result.y = temp);
}

// arm needs to be persistent
template <size_t N>
diff_map<N, 2> arm2d_func(const arm2d<N> &arm) {
  diff_map<N, 2> map;

  map.value = [&arm](vector2f &result, const vectorf<N> &state) {
    arm2d_get_joint(result, arm, state, N);
  };

  map.deriv = [&arm](matrixf<2, N> &result, const vectorf<N> &state) {
    vector2f partial;
    for (size_t i = 0; i < N; i++) {
      arm2d_get_partial(partial, arm, state, i);
      set_col(result, i, partial);
    }
  };
  //map.deriv = diff_sample(map.value).deriv;

  return map;
}


#endif

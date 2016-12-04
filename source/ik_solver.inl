#ifndef _IK_SOLVER_INL
#define _IK_SOLVER_INL


template <size_t N, size_t M>
diff_map<N, M> diff_sample(
    std::function<void(vectorf<M> &, const vectorf<N> &)> func) {

  diff_map<N, M> map;
  map.value = func;

  map.deriv = [func](matrixf<M, N> &result, const vectorf<N> &state) {
    lfloat delta = 0.001f;
    vectorf<N> state1;
    vectorf<M> value0, value1;
    func(value0, state);

    for (size_t j = 0; j < N; j++) {
      state1 = state;
      state1.data[j] += delta;
      func(value1, state1);

      sub(value1, value1, value0);
      mul(value1, value1, 1/delta);
      set_col(result, j, value1);
    }
  };

  return map;
}


template <size_t N, size_t M>
void ik_solve(vectorf<N> &current, diff_map<N, M> func) {
}


#endif

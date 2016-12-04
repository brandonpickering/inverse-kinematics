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
void ik_solve(vectorf<N> &current, diff_map<N, M> func,
    const vectorf<M> &target) {
  for (int i = 0; i < 100; i++) {
    vectorf<M> value;
    func.value(value, current);

    matrixf<M, N> deriv;
    func.deriv(deriv, current);

    vectorf<M> evderiv;
    sub(evderiv, value, target);
    mul(evderiv, evderiv, (lfloat) 2);
    matrixf<1, M> ederiv;
    set_row(ederiv, 0, evderiv);

    matrixf<1, N> gradient;
    mul(gradient, ederiv, deriv);

    vectorf<N> dstate;
    get_row(dstate, gradient, 0);
    mul(dstate, dstate, (lfloat) -0.01);

    add(current, current, dstate);
  }
}


#endif

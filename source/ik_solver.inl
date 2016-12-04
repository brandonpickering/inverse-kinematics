#ifndef _IK_SOLVER_INL
#define _IK_SOLVER_INL


template <size_t N, size_t M>
diff_map<N, M> diff_sample(
    std::function<void(vectorf<M> &, const vectorf<N> &)> func) {

  diff_map<N, M> map;
  map.value = func;

  map.deriv = [func](matrixf<M, N> &result, const vectorf<N> &state) {
    static vectorf<N> state1;
    static vectorf<M> value0, value1;
    const lfloat delta = 0.001f;
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
void ik_random_step(vectorf<N> &step, const vectorf<N> &current,
    diff_map<N, M> func, const vectorf<M> &target) {
  for (size_t i = 0; i < N; i++) {
    step.data[i] = (lfloat) 2*rand() / RAND_MAX - 1;
  }
}


template <size_t N, size_t M>
void ik_gradient_descent_step(vectorf<N> &step, const vectorf<N> &current,
    diff_map<N, M> func, const vectorf<M> &target) {
  static matrixf<M, N> deriv;
  static vectorf<M> ederiv;
  static vectorf<M> value;

  func.value(value, current);
  func.deriv(deriv, current);

  sub(ederiv, value, target);
  mul(ederiv, ederiv, (lfloat) 2);

  mul(row_vector(step), row_vector(ederiv), deriv);
  mul(step, step, (lfloat) -1);
}


template <size_t N, size_t M>
void ik_solve(vectorf<N> &current, diff_map<N, M> func,
    const vectorf<M> &target) {
  static vectorf<N> step;
  static vectorf<N> next;
  static vectorf<M> value;

  for (int k = 0; k < 1000; k++) {
    func.value(value, current);
    lfloat error = square_dist(value, target);
    if (error < (lfloat) 0.0001) break;

    ik_random_step(step, current, func, target);
    comp_clamp_scale(step, step, (lfloat) 0.01);

    bool improve = false;
    for (int k2 = 0; k2 < 100; k2++) {
      add(next, current, step);
      func.value(value, next);
      if (square_dist(value, target) < error) {
        improve = true;
        break;
      }
      mul(step, step, (lfloat) 0.5);
    }
    if (improve) current = next;
    else break;
  }
}


#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ik_solver.hpp"
#include "linalg.hpp"
#include "utils.hpp"


// TODO: Can easily extend an arm to have multiple hands, using tree structure


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

  return map;
}


arm2d<4> arm = {
  {0, 0},
  {0.4, 0.3, 0.2, 0.1},
};
vectorf<4> state = {0, 0, 0, 0};

vector2f mouse_pos;

void update(lfloat dt) {
  (void) dt;
  static diff_map<4, 2> func = arm2d_func(arm);
  ik_solve(state, func, mouse_pos);

  for (size_t i = 0; i < 4; i++)
    state.data[i] = angle_rep(state.data[i]);
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1, 1, 1);
  glBegin(GL_LINE_STRIP);
  vector2f point;
  for (size_t i = 0; i <= 4; i++) {
    arm2d_get_joint(point, arm, state, i);
    glVertex2f(point.x, point.y);
  }
  glEnd();
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action,
    int mods) {
  (void) scancode;
  (void) mods;
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}


#include <unistd.h>


int main() {
  glfwInit();

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow *window = glfwCreateWindow(480, 480, "", NULL, NULL);
  glfwSetKeyCallback(window, &keyCallback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) &glfwGetProcAddress);

  double last_time = glfwGetTime();
  double fps_time = glfwGetTime();
  int frames = 0;

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouse_pos.x = 2*mouseX / width - 1;
    mouse_pos.y = 1 - 2*mouseY / height;

    double dt = glfwGetTime() - last_time;
    last_time = glfwGetTime();
    update(dt);

    glViewport(0, 0, width, height);

    render();

    glfwSwapBuffers(window);
    glfwPollEvents();

    frames++;
    if (glfwGetTime() - fps_time >= 1) {
      fps_time = glfwGetTime();
      std::string fps = stringf("%d", frames);
      glfwSetWindowTitle(window, fps.data());
      frames = 0;
    }
  }

  glfwTerminate();
  return 0;
}

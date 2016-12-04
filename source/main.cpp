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
/*template <size_t N>
diff_map<N, 2> arm2d_func(const arm2d<N> &arm) {
  diff_map<N, 2> map;

  map.value = [&arm](vectorf<N> state) {
    resturn 
  };

  return map;
}*/


arm2d<4> arm = {
  {0, 0},
  {0.4, 0.3, 0.2, 0.1},
};
vectorf<4> state = {0, 0, 0, 0};

void update(lfloat dt) {
  for (size_t i = 0; i < 4; i++)
    state.data[i] += 0.5 * dt;
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);

  vector2f point;
  vector2f deriv;

  glColor3f(1, 1, 1);
  glBegin(GL_LINE_STRIP);
  for (size_t i = 0; i <= 4; i++) {
    arm2d_get_joint(point, arm, state, i);
    glVertex2f(point.x, point.y);
  }
  glEnd();

  glColor3f(1, 0, 0);
  glBegin(GL_LINE_STRIP);
  size_t joint = 1;
  arm2d_get_joint(point, arm, state, joint);
  glVertex2f(point.x, point.y);
  arm2d_get_joint(point, arm, state, 4);
  glVertex2f(point.x, point.y);
  arm2d_get_partial(deriv, arm, state, joint);
  add(point, point, deriv);
  glVertex2f(point.x, point.y);
  glEnd();
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action,
    int mods) {
  (void) scancode;
  (void) mods;
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main() {
  glfwInit();

  glfwSwapInterval(1);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow *window = glfwCreateWindow(480, 480, "", NULL, NULL);
  glfwSetKeyCallback(window, &keyCallback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) &glfwGetProcAddress);

  double lastTime = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    double dt = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();
    update(dt);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

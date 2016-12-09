#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "arm2d.hpp"
#include "arm3d.hpp"
#include "ik_solver.hpp"
#include "linalg.hpp"
#include "utils.hpp"


/*
const size_t num_links = 4;

arm2d<num_links> arm = {
  {0, 0},
  {0.4, 0.3, 0.2, 0.1, 0.5},
};
vectorf<num_links> state = {0, 0, 0, 0};
*/


const size_t num_links = 4;

arm3d<num_links> arm = {
  {0, 0, 0},
  {4, 3, 2, 2},
};

vectorf<3*num_links> state = {};


lfloat runtime = 0;

vector2f mouse_pos;

vector3f target;
vector3f target_move;
int misc_inp[3];


vector3f target_func(lfloat t) {
  lfloat len = 0;
  for (size_t i = 0; i < num_links; i++)
    len += arm.lengths[i];

  return {len, 0, 0};
}


void update(lfloat dt) {
  runtime += dt;

  vector3f targ_vel;
  normalize(targ_vel, target_move);
  mul(targ_vel, targ_vel, 5 * dt);
  add(target, target, targ_vel);

  //for (int i = 0; i < 3; i++)
    //state.data[2*3 + i] += misc_inp[i]*dt;

  //target = target_func(runtime);

  static diff_map<3*num_links, 3> func = arm3d_func(arm);
  //if (false)
    ik_solve(state, func, target);
}

void render() {
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 1, 100);
  glTranslatef(0, -5, -12);


  static diff_map<3*num_links, 3> func = arm3d_func_samp(arm);
  static diff_map<3*num_links, 3> func2 = arm3d_func(arm);


  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.3f, 0.3f, 0.3f);
  glBegin(GL_LINES);
  const int span = 10;
  for (int i = -span; i <= span; i++) {
    glVertex3f((float) i, 0, (float) -span);
    glVertex3f((float) i, 0, (float) span);
    glVertex3f((float) -span, 0, (float) i);
    glVertex3f((float) span, 0, (float) i);
  }
  glEnd();

  glColor3f(0, 1, 0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(target.x, target.y, target.z);
  for (int i = 0; i <= 10; i++) {
    float r = 0.1f;
    float a = 2 * 3.1415926 * (float) i / 10;
    glVertex3f(target.x + r*std::cos(a), target.y + r*std::sin(a), target.z);
  }
  glEnd();

  glColor3f(1, 1, 1);
  glBegin(GL_LINE_STRIP);
  vector3f point;
  for (size_t i = 0; i <= num_links; i++) {
    arm3d_get_joint(point, arm, state, i);
    glVertex3f(point.x, point.y, point.z);
  }
  glEnd();

  /*
  glColor3f(1, 0, 0);
  size_t index = 0;
  static matrix<lfloat, 3, 3*num_links> deriv;
  func.deriv(deriv, state);
  static vectorf<3*num_links> samp;
  for (size_t i = 0; i < 3*num_links; i++) samp.data[i] = 0;
  samp.data[3*index] = 1;
  vector3f partial;
  mul(partial, deriv, samp);
  add(partial, partial, point);
  glBegin(GL_LINES);
  glVertex3f(point.x, point.y, point.z);
  glVertex3f(partial.x, partial.y, partial.z);
  glEnd();

  glColor3f(0, 1, 0);
  func2.deriv(deriv, state);
  for (size_t i = 0; i < 3*num_links; i++) samp.data[i] = 0;
  samp.data[3*index] = 1;
  mul(partial, deriv, samp);
  add(partial, partial, point);

  glBegin(GL_LINES);
  glVertex3f(point.x + 0.2, point.y, point.z);
  glVertex3f(partial.x + 0.2, partial.y, partial.z);
  glEnd();
  */

  /*
  glColor3f(1, 0, 0);
  cross(partial, point, {1, 0, 0});
  mul(partial, partial, (lfloat) -1);
  add(partial, partial, point);
  glBegin(GL_LINES);
  glVertex3f(point.x, point.y, point.z);
  glVertex3f(partial.x, partial.y, partial.z);
  glEnd();

  glColor3f(0, 1, 0);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(state.data[0], state.data[1], state.data[2]);
  glEnd();
  */
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action,
    int mods) {
  (void) scancode;
  (void) mods;

  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (action != GLFW_PRESS && action != GLFW_RELEASE) return;

  switch (key) {
    case GLFW_KEY_RIGHT:
      target_move.x = action == GLFW_PRESS ? 1 : 0;
      break;
    case GLFW_KEY_LEFT:
      target_move.x = action == GLFW_PRESS ? -1 : 0;
      break;
    case GLFW_KEY_UP:
      if (action == GLFW_RELEASE)
        target_move.y = target_move.z = 0;
      else if (mods & GLFW_MOD_SHIFT)
        target_move.y = 1;
      else
        target_move.z = -1;
      break;
    case GLFW_KEY_DOWN:
      if (action == GLFW_RELEASE)
        target_move.y = target_move.z = 0;
      else if (mods & GLFW_MOD_SHIFT)
        target_move.y = -1;
      else
        target_move.z = 1;
      break;

    case GLFW_KEY_A:
      misc_inp[0] = action == GLFW_PRESS ? (mods & GLFW_MOD_SHIFT ? -1 : 1) : 0;
      break;
    case GLFW_KEY_S:
      misc_inp[1] = action == GLFW_PRESS ? (mods & GLFW_MOD_SHIFT ? -1 : 1) : 0;
      break;
    case GLFW_KEY_D:
      misc_inp[2] = action == GLFW_PRESS ? (mods & GLFW_MOD_SHIFT ? -1 : 1) : 0;
      break;
  }
}


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

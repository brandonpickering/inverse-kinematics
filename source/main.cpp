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

vectorf<3*num_links> state;


lfloat runtime = 0;

vector2f mouse_pos;

vector3f target;
vector3f target_move;
int misc_inp[3];


vector3f target_func(lfloat t) {
  vector3f result;
  t = t*0.7 + 5;

  result.x = 0.8 * std::cos(t);
  result.y = std::sin(0.1 * t);
  result.y *= result.y;
  result.z = 0.5 * std::sin(0.5 * t);

  lfloat s = (t - 13.5) / 5;
  if (s >= 0 && s <= 1)
    mul(result, result, std::cos(s * PI));
  if (s > 1) mul(result, result, (lfloat) -1);
  if (s > 0.5) result.y += s - 0.5;

  lfloat len = 0;
  for (size_t i = 0; i < num_links; i++)
    len += arm.lengths[i];
  mul(result, result, len);
  return result;
}


void update(lfloat dt) {
  dt = std::min(dt, 1/30.0);
  runtime += dt;

  //vector3f targ_vel;
  //normalize(targ_vel, target_move);
  //mul(targ_vel, targ_vel, 5 * dt);
  //add(target, target, targ_vel);

  //for (int i = 0; i < 3; i++)
    //state.data[2*3 + i] += misc_inp[i]*dt;

  target = target_func(runtime);

  static diff_map<3*num_links, 3> func = arm3d_func(arm);
  //if (false)
    ik_solve(state, func, target);
}


void draw_segment(float len) {
  float rad = 0.3;
  int num_sides = 4;

  glColor3f(1, 1, 1);
  for (int i = 0; i < num_sides; i++) {
    float a0 = 2*PI * i / num_sides;
    float a1 = 2*PI * (i+1) / num_sides;
    float y0 = rad * std::cos(a0), y1 = rad * std::cos(a1);
    float z0 = rad * std::sin(a0), z1 = rad * std::sin(a1);
    vector3<float> normal = {0, y0+y1, z0+z1};
    normalize(normal, normal);

    glNormal3f(normal.x, normal.y, normal.z);
    glBegin(GL_TRIANGLES);
    glVertex3f(0, y0, z0);
    glVertex3f(0, y1, z1);
    glVertex3f(len, 0, 0);//y1, z1);
    //glVertex3f(len, y0, z0);
    glEnd();
  }
}

void render() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 2, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  float pos[] = {3, 5, 15, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, &pos[0]);


  lfloat camrad = 20;
  lfloat camspeed = 0.2;
  vector3f camera = {
    camrad*std::sin(camspeed*runtime),
    5,
    camrad*std::cos(camspeed*runtime),
  };
  float roty = -PI/2 + std::atan2(camera.z, camera.x);
  glRotatef(roty * 180/PI, 0, 1, 0);
  glTranslatef(-camera.x, -camera.y, -camera.z);
  //glTranslatef(0, -5, -12);


  static diff_map<3*num_links, 3> func = arm3d_func_samp(arm);
  static diff_map<3*num_links, 3> func2 = arm3d_func(arm);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static bool initial = true;
  if (initial) {
    vector3f point;
    arm3d_get_joint(point, arm, state, num_links);
    lfloat dist = square_dist(point, target);
    if (dist > 0.01) return;
    initial = false;
  }


  glColor4f(1, 1, 1, 0.2f);
  glNormal3f(0, 1, 0);
  glBegin(GL_LINES);
  const int span = 10;
  for (int i = -span; i <= span; i++) {
    glVertex3f((float) i, 0, (float) -span);
    glVertex3f((float) i, 0, (float) span);
    glVertex3f((float) -span, 0, (float) i);
    glVertex3f((float) span, 0, (float) i);
  }
  glEnd();

  /*glColor3f(0, 1, 0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(target.x, target.y, target.z);
  for (int i = 0; i <= 10; i++) {
    float r = 0.1f;
    float a = 2 * 3.1415926 * (float) i / 10;
    glVertex3f(target.x + r*std::cos(a), target.y + r*std::sin(a), target.z);
  }
  glEnd();*/

  glDisable(GL_LIGHTING);
  glBegin(GL_LINE_STRIP);
  float targd = 0.5;
  for (float i = -targd; i <= targd; i += 0.1) {
    vector3f targ = target_func(runtime + i);
    glColor4f(0, 1, 0, (targd - std::abs(i))/targd);
    glVertex3f((float) targ.x, (float) targ.y, (float) targ.z);
  }
  glEnd();
  glColor3f(0, 1, 0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(target.x, target.y, target.z);
  for (int i = 0; i <= 10; i++) {
    float r = 0.1f;
    float a = 2 * 3.1415926 * (float) i / 10;
    vector3f eye;
    sub(eye, camera, target);
    vector3f lx; cross(lx, eye, {0,1,0}); normalize(lx, lx);
    vector3f ly; cross(ly, eye, lx); normalize(ly, ly);
    vector3f p = target;
    vector3f t; mul(t, lx, r * (lfloat) std::cos(a)); add(p, p, t);
    mul(t, ly, r * (lfloat) std::sin(a)); add(p, p, t);
    //glVertex3f(target.x + r*std::cos(a), target.y + r*std::sin(a), target.z);
    glVertex3f(p.x, p.y, p.z);
  }
  glEnd();
  glEnable(GL_LIGHTING);

  /*glColor3f(1, 1, 1);
  glBegin(GL_LINE_STRIP);
  vector3f point;
  for (size_t i = 0; i <= num_links; i++) {
    arm3d_get_joint(point, arm, state, i);
    glVertex3f(point.x, point.y, point.z);
  }
  glEnd();*/

  for (size_t i = 0; i < num_links; i++) {
    vector3f axis = subvec<lfloat, 3*num_links, 3>(state, 3*i);
    lfloat angle = std::sqrt(square_mag(axis));
    if (angle > 0.00001) mul(axis, axis, 1/angle);

    glRotatef(angle * 180/PI, axis.x, axis.y, axis.z);
    draw_segment((float) arm.lengths[i]);
    glTranslatef(arm.lengths[i], 0, 0);
  }

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
  glfwWindowHint(GLFW_SAMPLES, 4);
  GLFWwindow *window = glfwCreateWindow(480, 480, "", NULL, NULL);
  glfwSetKeyCallback(window, &keyCallback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) &glfwGetProcAddress);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

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

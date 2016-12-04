#include <cstdio>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linalg.hpp"
#include "utils.hpp"


void keyCallback(GLFWwindow *window, int key, int scancode, int action,
    int mods) {
  (void) scancode;
  (void) mods;
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main() {
  glfwInit();


  vector3f vec;
  vec.x = 3;
  vec.y = 2;
  vec.data[2] = 1;
  printf("%s\n", str(vec).c_str());

  matrixf<2, 3> mat;
  mat.data[0] = 2;
  mat.data[4] = 1;
  printf("%s", str(mat).c_str());

  vector2f vec2;
  mul(vec2, mat, vec);
  printf("%s\n", str(vec2).c_str());

  matrixf<3, 4> mat2;
  for (size_t i = 0; i < 12; i++)
    mat2.data[i] = (lfloat) rand() / RAND_MAX;
  printf("%s\n", str(mat2).c_str());

  matrixf<2, 4> mat3;
  mul(mat3, mat, mat2);
  printf("%s\n", str(mat3).c_str());

  matrixf<4, 2> mat4;
  transpose(mat4, mat3);
  printf("%s\n", str(mat4).c_str());


  GLFWwindow *window = glfwCreateWindow(480, 480, "", NULL, NULL);
  glfwSetKeyCallback(window, &keyCallback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) &glfwGetProcAddress);

  while (!glfwWindowShouldClose(window)) {

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

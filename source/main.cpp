#include <cstdio>

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
  printf("%s\n", str(vec).c_str());


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

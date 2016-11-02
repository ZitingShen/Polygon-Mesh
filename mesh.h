#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "initshader.h"
#include "fin.h"
#include "common.h"

#include <glm/gtc/matrix_transform.hpp>

void init(GLFWwindow* window);
void reshape(GLFWwindow* window, int w, int h);
void framebuffer_resize(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods);
void mouse(GLFWwindow *window, int button, int action, int mods);
void change_perspective(GLFWwindow* window);
void change_view();
void print();
#endif
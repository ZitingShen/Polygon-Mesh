#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include "initshader.h"
#include "fin.h"
#include "common.h"

#include <glm/gtc/matrix_transform.hpp>

typedef enum _p_mode{PARALLEL, PERSPECTIVE} p_mode;
typedef enum _s_mode{FLAT, SMOOTH, PHONG} s_mode;
typedef enum _d_mode{VERTEX, EDGE, FACE} d_mode;

void init(GLFWwindow* window);
void reshape(GLFWwindow* window, int w, int h);
void framebuffer_resize(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods);
void mouse(GLFWwindow *window, int button, int action, int mods);
static GLuint make_bo(GLenum type, const void *buf, GLsizei buf_size);
void draw_mesh();
void change_perspective(GLFWwindow* window);
void change_view();
void print();
#endif
#ifndef MESH_H
#define MESH_H

#include "view.h"
#include "gl_replacement.h"
#include "initshader.h"
#include "common.h"

void init(GLFWwindow* window);
void reshape(GLFWwindow* window, int w, int h);
void framebuffer_resize(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods);
static GLuint make_bo(GLenum type, const void *buf, GLsizei buf_size);
#endif
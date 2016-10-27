#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <GLFW/glfw3.h>
#include <OpenGL/glu.h>
#else
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define RADIAN_TO_DEGREE          57.29578 // 180 over pi
#define	DEGREE_TO_RADIAN		  0.00873

#define CAMERA_NEAR               2
#define CAMERA_FAR                100000000
#define ZOOM_STEP_RATIO			  0.05
#define INITIAL_X_DISPLACEMENT    0.2
#define INITIAL_Y_DISPLACEMENT    0.5

#endif
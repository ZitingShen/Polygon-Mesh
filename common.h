#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#define RADIAN_TO_DEGREE          57.29578f // 180 over pi
#define	DEGREE_TO_RADIAN		      0.00873f

#define CAMERA_NEAR               0.1f
#define CAMERA_FAR                100000000.0f
#define ZOOM_STEP_RATIO			      0.3f
#define EYE_X_DISPLACEMENT        1.8f
#define EYE_Y_DISPLACEMENT        5.0f
#define EYE_Z_DISPLACEMENT        1.2f
#define LIGHT_X                   10.0f
#define LIGHT_Y                   10.0f
#define LIGHT_Z                   10.0f
#define MESH_X                    0.7f
#define MESH_Y                    0.7f
#define MESH_Z                    0.8f
#define BLOCK                     2.0f
#define ROTATE_STEP_X             3.0f
#define ROTATE_STEP_Z             5.0f

#define POS_LOCATION              0
#define NORMAL_LOCATION           1

static GLfloat material_props[][10] = {
  {0.2, 0.2, 0.2, 0.8, 0.8, 0.8, 0.0, 0.0, 0.0, 0.0},
  {0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633,
0.6},
  {0.135, 0.2225, 0.1575, 0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1},
  {0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634,
0.346435, 0.3},
  {0.25, 0.20725, 0.20725, 1, 0.829, 0.829, 0.296648, 0.296648, 0.296648,
0.088},
  {0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959,
0.626959, 0.6},
  {0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829,
0.306678, 0.1},
  {0.329412, 0.223529, 0.027451, 0.780392, 0.568627, 0.113725, 0.992157,
0.941176, 0.807843, 0.21794872},
  {0.2125, 0.1275, 0.054, 0.714, 0.4284, 0.18144, 0.393548, 0.271906,
0.166721, 0.2},
  {0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6},
  {0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622,
0.086014, 0.1},
  {0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802,
0.366065, 0.4},
  {0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273,
0.508273, 0.5065, 0.4},
  {0.0, 0.0, 0.0, 0.01, 0.01, 0.01, 0.50, 0.50, 0.50, .25},
  {0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392, 0.50196078, 0.50196078,
0.50196078, .25},
  {0.0, 0.0, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25},
  {0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.6, 0.6, .25},
  {0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, .25},
  {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.60, 0.60, 0.50, .25},
  {0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.60, 0.60, 0.7, .25},
  {0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.60, 0.60, 0.7, .25},
  {0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.4, 0.4, 0.4, .078125},
  {0.0, 0.05, 0.05, 0.4, 0.5, 0.5, 0.04, 0.7, 0.7, .078125},
  {0.0, 0.05, 0.0, 0.4, 0.5, 0.4, 0.04, 0.7, 0.04, .078125},
  {0.05, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, .078125},
  {0.05, 0.05, 0.05, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7, .078125},
  {0.05, 0.05, 0.0, 0.5, 0.5, 0.4, 0.7, 0.7, 0.04, .078125},
  {0.0, 0.0, 0.05, 0.4, 0.4, 0.5, 0.04, 0.04, 0.7, .078125},
  {0.15, 0.15, 0.0, 0.5, 0.5, 0.0, 0.7, 0.7, 0.7, 0.3},
  {0.15, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.7, 0.7, 0.3},
  {0.0, 0.15, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25},
  {0.29225, 0.29225, 0.29225, 0.50754, 0.50754, 0.50754, 0.508273,
   0.508273, 0.5065, 0.5},
  {0.15, 0.0, 0.15, 0.5, 0.0, 0.5, 0.60, 0.60, 0.7, .3},
  /* {0.05, 0.05, 0.15, 0.2, 0.2, 0.3, 0.0, 0.0, 0.3, 0.25} */
  {0.05, 0.05, 0.15, 0.2, 0.2, 0.5, 0.0, 0.0, 0.3, 0.078},
  {0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.60, 0.60, 0.7, .25}
};
enum MaterialType { MAT_DEFAULT, MAT_EMERALD, MAT_JADE, MAT_OBSIDIAN,
                    MAT_PEARL, MAT_RUBY, MAT_TURQUOISE, MAT_BRASS,
                    MAT_BRONZE, MAT_CHROME, MAT_COPPER, MAT_GOLD, MAT_SILVER,
                    MAT_BLACK_PLASTIC, MAT_CYAN_PLASTIC, MAT_GREEN_PLASTIC,
                    MAT_RED_PLASTIC, MAT_WHITE_PLASTIC, MAT_YELLOW_PLASTIC,
                    MAT_BLUE_PLASTIC, MAT_MAGENTA_PLASTIC, MAT_BLACK_RUBBER,
                    MAT_CYAN_RUBBER, MAT_GREEN_RUBBER, MAT_RED_RUBBER,
                    MAT_WHITE_RUBBER, MAT_YELLOW_RUBBER, MAT_BLUE_RUBBER,
                    MAT_BRIGHT_YELLOW, MAT_BRIGHT_RED, MAT_BRIGHT_GREEN,
                    MAT_BRIGHT_SILVER, MAT_BRIGHT_MAGENTA,
                    MAT_SURFACE_BLUE, MAT_NEW_SURFACE_BLUE};



#endif
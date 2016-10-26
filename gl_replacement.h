#ifndef GL_REPLACEMENT_H
#define GL_REPLACEMENT_H

#include "common.h"
#include "my_matrix.h"

void myLookAt(GLfloat current_matrix[], GLfloat eye[3], GLfloat center[3], GLfloat up[3]);
void myPerspective(GLfloat proj_mat[], GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
void myTranslate(GLfloat current_matrix[], GLfloat x, GLfloat y, GLfloat z);
void myRotate(GLfloat current_matrix[], GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void myScale(GLfloat current_matrix[], GLfloat x, GLfloat y, GLfloat z);

#endif

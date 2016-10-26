#include "gl_replacement.h"

void myLookAt(GLfloat mv_mat[], GLfloat eye[3], GLfloat centre[3], GLfloat up[3]){
  vec3 forward_normal = normalise(vec3(centre[0] - eye[0],
                                       centre[1] - eye[1],
                                       centre[2] - eye[2]));
  vec3 side_normal = normalise(cross(forward_normal, vec3(up[0],up[1],up[2])));

  vec3 up_normal = cross(side_normal, forward_normal);

  GLfloat trans_matrix[16] = //putting in column-major order
             {side_normal[0], up_normal[0], -forward_normal[0], 0,
              side_normal[1], up_normal[1], -forward_normal[1], 0,
              side_normal[2], up_normal[2], -forward_normal[2], 0,
              0,0,0,1};

  //glLoadMatrixf(trans_matrix);
  //glTranslatef(-eye[0], -eye[1], -eye[2]);
  myTranslate(trans_matrix, -eye[0], -eye[1], -eye[2]); //move to eye
  memcpy(mv_mat, trans_matrix, sizeof(GLfloat)*16);
}

void myPerspective(GLfloat proj_mat[], GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar){
  GLfloat y_max = tan(fovy * DEGREE_TO_RADIAN) * zNear;
  GLfloat y_min = -y_max;
  GLfloat x_max = y_max * aspect;
  GLfloat x_min = -x_max;

  GLfloat trans_matrix[16] = //putting in column-major order
       {2*zNear*aspect/(x_max-x_min), 0, 0, 0,
        0, 2*zNear/(y_max-y_min), 0, 0,
        0, 0, -(zFar+zNear)/(zFar-zNear), -1,
        0, 0, -2*(zFar*zNear)/(zFar-zNear),0};

  memcpy(proj_mat, trans_matrix, sizeof(GLfloat)*16);
}

void myTranslate(GLfloat current_matrix[], GLfloat x, GLfloat y, GLfloat z) {
  mat4 current(current_matrix);
  mat4 trans;
  identity_mat4(trans);
  trans[3] = x;
  trans[7] = y;
  trans[11] = z;
  trans = multiply(current, trans);
  trans.transpose();
  unpack(trans, current_matrix);
}

void myRotate(GLfloat current_matrix[], GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
  GLfloat c = cos(angle);
  GLfloat s = sin(angle);
  vec3 axis(x, y, z);

  axis = normalise(axis);
  x = axis[0];
  y = axis[1];
  z = axis[2];
  mat4 current(current_matrix);
  mat4 trans;
  identity_mat4(trans);
  trans[0] = x*x*(1 - c) + c;
  trans[1] = x*y*(1 - c) - z*s;
  trans[2] = x*z*(1 - c) + y*s;
  trans[4] = y*x*(1 - c) + z*s;
  trans[5] = y*y*(1 - c) + c;
  trans[6] = y*z*(1 - c) - x*s;
  trans[8] = x*z*(1 - c) - y*s;
  trans[9] = y*z*(1 - c) + x*s;
  trans[10] = z*z*(1 - c) + c;
  trans = multiply(current, trans);
  trans.transpose();
  unpack(trans, current_matrix);
}

void myScale(GLfloat current_matrix[], GLfloat x, GLfloat y, GLfloat z) {
  mat4 current(current_matrix);
  mat4 trans;
  identity_mat4(trans);
  trans[0] = x;
  trans[5] = y;
  trans[10] = z;
  trans = multiply(current, trans);
  trans.transpose();
  unpack(trans, current_matrix);
}

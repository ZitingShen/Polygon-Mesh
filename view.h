#ifndef VIEW_H
#define VIEW_H

#include "common.h"
#include "list.h"
#include "goal.h"
#include "boid.h"

const GLfloat CLEAR_COLOR[3] = {0.182, 0.008, 0.235};			       

const float CHESS_BOARD_COLOUR_X[3] = {0.449,0.451,0.494};  //grey
const float CHESS_BOARD_COLOUR_Y[3] = {0.0, 0.0, 0.0};     //black

typedef enum viewMode {DEFAULT, TRAILING, SIDE} viewMode;

void change_view(GLfloat mv_mat[], viewMode viewmode, List *flock, GOAL *goal, int index);

void update_background(GLfloat squares_grey[], GLfloat squares_black[],
		     GLfloat mv_mat[]);
#endif

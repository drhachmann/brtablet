
#ifndef _GUI_COMMON_H_
#define _GUI_COMMON_H_

#define NO_RECORDED 0
#define RECORDED 1

/*Rows of matrix calibration*/
#define N_ROW  5

/*Columns of matrix calibration*/
#define N_COLUMN 5

#include "util.h"

typedef struct{
	Point point_xy[N_ROW][N_COLUMN];
	Point point_uv[N_ROW][N_COLUMN];
	int recorded[N_ROW][N_COLUMN];
	short current_X, current_Y;
	int win_width, win_height;
}GuiInfo, *GuiInfoPtr;

typedef enum {
	Key_error,
	Key_Down,
	Key_Up,
	Key_Left,
	Key_Right,
	Key_Enter,
	Key_Esc,
	Key_w
}CommonEvent;

 void key_common_handle(GuiInfoPtr gui, CommonEvent event);

#endif

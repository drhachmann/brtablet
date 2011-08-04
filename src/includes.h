#ifndef _INCLUDE_H_
#define _INCLUDE_H_


typedef struct{
	int x;
	int y;
	int z;
}Point;


#ifndef _BRCALIB_H_
#define _BRCALIB_H_

#define MIN_U 0
//1650
#define MAX_U 3750
#define MIN_V 0
//600
#define MAX_V 2800

#define N_ROW 5
#define N_COLUMN 5


#define N_PONTOS N_ROW * N_COLUMN 

#define NO_RECORDED 0
#define RECORDED 1

#define for_each(var, lim) \
	for(var=0; var<lim; var++)\
	

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768






typedef struct _BrCalib{
	Point point_xy[N_ROW][N_COLUMN];
	Point point_uv[N_ROW][N_COLUMN];
	Point xy[N_PONTOS];
	Point uv[N_PONTOS];
	int index;
	char record[N_PONTOS];
	int recorded[N_ROW][N_COLUMN];
	short current_X, current_Y;
	short res_X, res_Y, res_U, res_V;
	Point uv_to_xy[2000][2000];
	double coef[N_PONTOS][2][3];

	
}BrCalib, *BrCalibPtr;

void point_interpolation(Point *p, BrCalib *brCalib);
void convert_XY_UV(Point point_uv[4], Point point_xy[4], Point *uv, Point *xy);
void point_interpolation_all(BrCalibPtr brCalib);
void uv_to_xy_novo(BrCalib *brCalib, Point *uv, Point *xy);
void uv_to_xy_velho(BrCalib *brCalib, Point *uv, Point *xy);
#endif

#endif

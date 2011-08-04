#ifndef _CALIB_H_
#define _CALIB_H_

void get_coeficientes(Point point_uv[3], Point point_xy[3], double coef[][3]);
void from_uv_to_xy(double coef[][3], Point *uv, Point* p);

#endif

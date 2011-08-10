/*
 * Copyright (c) 2011 Diego Rodrigo Hachmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include "util.h"

static int transposta(double mtx[3][3], double resp[3][3]){
	int i, j;
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			resp[i][j]=mtx[j][i];
		}
	}
}

static int cofatores(double mtx[3][3], double resp[3][3]){
	int i,j;
	for(i=0;i<3; i++){
		for(j=0; j<3; j++){
			resp[i][j] = (mtx[(i-1+3)%3][(j-1+3)%3]*mtx[(i+1+3)%3][(j+1+3)%3] - 
						 mtx[(i-1+3)%3][(j+1+3)%3]*mtx[(i+1+3)%3][(j-1+3)%3]);
		}
	}
}

static double determinante(double mtx[3][3]){
	return((mtx[0][0]*mtx[1][1]*mtx[2][2] + mtx[1][0] * mtx[2][1]*mtx[0][2] + mtx[2][0]*mtx[0][1]*mtx[1][2])
		  -(mtx[2][0]*mtx[1][1]*mtx[0][2] + mtx[1][0]*mtx[0][1]*mtx[2][2] + mtx[0][0]*mtx[2][1]*mtx[1][2]));
}

static void inversa(double mtx[3][3], double resp[3][3]){
	int i, j;
	double coef[3][3];
	double adj[3][3];
	double determ;
	cofatores(mtx, coef);
	transposta(coef, adj);
	determ = determinante(mtx);
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			resp[i][j] = adj[i][j]/determ;
	
}

/*alfaX, betaX, deltaX
  alfaY, betaY, deltaY*/
void get_coeficientes(Point point_uv[3], Point point_xy[3], double coef[][3]){
	int i, j, k;
	double mtx[3][3], inver[3][3];
	mtx[0][0]=point_uv[0].x; mtx[0][1] = point_uv[0].y; mtx[0][2]=1;
	mtx[1][0]=point_uv[1].x; mtx[1][1] = point_uv[1].y; mtx[1][2]=1;
	mtx[2][0]=point_uv[2].x; mtx[2][1] = point_uv[2].y; mtx[2][2]=1;
	inversa(mtx, inver);
	for(i=0; i<3; i++)
		for(j=0; j<3; j++){
			coef[0][i]=coef[1][i]=0;
			for(k=0; k<3; k++){
				coef[0][i] += inver[i][k]*point_xy[k].x;
				coef[1][i] += inver[i][k]*point_xy[k].y;
			}
		}
}

void from_uv_to_xy(double coef[][3], Point *uv, Point* p){
	p->x = coef[0][0]*uv->x + coef[0][1]*uv->y + coef[0][2];
	p->y = coef[1][0]*uv->x + coef[1][1]*uv->y + coef[1][2];

}



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
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "io.h"
#include "util.h"


int get_device_current_coord(Point *uv){
	char buff[100];
	int sim_fd = open(PATH_SYSFS_POINT, O_RDWR);
	if(sim_fd<0){
		return handle_error_file_open(PATH_SYSFS_POINT);

	}
	read(sim_fd, buff, sizeof(buff));
	sscanf(buff, "%d %d %d", &uv->x, &uv->y, &uv->z);
	printf("%d %d %d\n", uv->x, uv->y, uv->z);
	close(sim_fd);
}

int write_calib_uvz(Point uv[N_ROW][N_COLUMN]){
	int i, j;
	FILE *fp = fopen(PATH_FILE_UVZ, "w");
	if(fp==NULL){
		handle_error_file_open(PATH_FILE_UVZ);
		exit (-1);
	}
	for(i=0; i<N_ROW; i++){
		for(j=0; j< N_COLUMN; j++){
			fprintf(fp, "%d %d %d ", (int)uv[i][j].x, (int)uv[i][j].y, (int)uv[i][j].z);
		}
	}
	fclose(fp);
}

int write_calib_mtx(Point uv[][N_COLUMN], Point xy[][N_COLUMN]){
	int i, j, w, k;
	double coef[3][3];
	Point _uv[3], _xy[3];
	FILE *fp = fopen(PATH_FILE_MTX, "w");
	if(fp==NULL){
		return handle_error_file_open(PATH_FILE_MTX);
	}
	for(i=0; i<N_ROW-1; i++){
		for(j=0; j<N_COLUMN-1; j++){
			//par
			_uv[0] = uv[i][j]; _uv[1] = uv[i][j+1]; _uv[2]=uv[i+1][j+1];
			_xy[0] = xy[i][j]; _xy[1] = xy[i][j+1]; _xy[2]=xy[i+1][j+1];
			memset(coef, 0, sizeof(coef));
			get_coeficientes(_uv, _xy, coef);				
			for(w=0; w<3; w++)
				for(k=0; k<3; k++)
					fprintf(fp, "%d ", (int)(coef[w][k]*(1<<N_SHIFT)));

			//impar
			_uv[0] = uv[i+1][j+1]; _uv[1] = uv[i+1][j]; _uv[2]=uv[i][j];	
			_xy[0] = xy[i+1][j+1]; _xy[1] = xy[i+1][j]; _xy[2]=xy[i][j];	
			memset(coef, 0, sizeof(coef));
			get_coeficientes(_uv, _xy, coef);	
			for(w=0; w<3; w++)
				for(k=0; k<3; k++)
					fprintf(fp, "%d ", (int)(coef[w][k]*(1<<N_SHIFT)));
		}
	}
	fclose(fp);
}

int mtx_to_driver(){
	int sim_fd = open(PATH_SYSFS_DATA, O_RDWR);
	if(sim_fd<0){
		return handle_error_file_open(PATH_SYSFS_DATA);
	}
	int i, j,  w, temp;
	char buff[500];
	FILE *fp = fopen(PATH_FILE_MTX, "r");
	if(fp==NULL){
		return handle_error_file_open(PATH_FILE_MTX);
		
	}
	int ini=0;
	memset(buff, ' ', sizeof(buff));
	for(i=0; i<4*4*2; i++){
		for(j=0; j<3; j++){
			for(w=0; w<3; w++){
				if(fscanf(fp, "%d", &temp)<1){
					return handle_error_bad_file(PATH_FILE_MTX);
				}
				sprintf(buff, "%d", temp);
			}
		}
		int fim = ftell(fp);
		fseek (fp , ini , SEEK_SET);
		if(fread(buff, 1, fim-ini+1, fp)<fim-ini+1)return -1;
		buff[fim-ini+1]=0;
		fseek (fp , fim , SEEK_SET);
		write(sim_fd, buff, fim-ini+1);
		ini = fim+1;
		fsync(sim_fd);	
	}
	fclose(fp);
	close(sim_fd);
}

int uvz_to_driver(){
	int sim_fd = open(PATH_SYSFS_DATA, O_RDWR);
	if(sim_fd<0){
		return handle_error_file_open(PATH_SYSFS_DATA);
	}

	char buff[500];
	memset(buff, ' ', sizeof(buff));
	FILE *fp = fopen(PATH_FILE_UVZ, "r");
	if(fp == NULL){
		return handle_error_file_open(PATH_SYSFS_DATA);
	}
	fseek (fp , 0L , SEEK_END);
	int fim = ftell(fp);
	fseek (fp , 0L , SEEK_SET);
	if(fread(buff, 1, fim, fp)<fim){
		return handle_error_bad_file(PATH_SYSFS_DATA);

	}
	buff[fim]=0;
	write(sim_fd, buff, fim);
	fsync(sim_fd);	
	fclose(fp);
	close(sim_fd);
}


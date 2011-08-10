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
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "io.h"
#include "util.h"

static char *devices[] = {"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3",
								      "/dev/ttyS4", "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2", NULL};


int get_device_current_coord(Point *uv){
	char buff[100];
	int sim_fd = open(PATH_SYSFS_POINT, O_RDWR);
	if(sim_fd<0){
		return handle_error_file_open(PATH_SYSFS_POINT);

	}
	read(sim_fd, buff, sizeof(buff));
	sscanf(buff, "%d %d %d", &uv->x, &uv->y, &uv->z);
	close(sim_fd);
}

int write_calib_uvz(Point uv[N_ROW][N_COLUMN]){
	int i, j;
	FILE *fp = fopen(PATH_FILE_UVZ, "w");
	if(fp==NULL){
		return -1;
	}
	for(i=0; i<N_ROW; i++){
		for(j=0; j< N_COLUMN; j++){
			fprintf(fp, "%d %d %d ", (int)uv[i][j].x, (int)uv[i][j].y, (int)uv[i][j].z);
		}
	}
	fclose(fp);
	return(0);
}


int write_calib_mtx(Point uv[][N_COLUMN], Point xy[][N_COLUMN]){
	int i, j, w, k;
	double coef[3][3];
	Point _uv[3], _xy[3];
	FILE *fp = fopen(PATH_FILE_MTX, "w");
	if(fp==NULL){
		//return handle_error_file_open(PATH_FILE_MTX);
		return (-1);
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
	return (0);
}

int driver_set_operation(char *drv, int op, int val){
	int sim_fd = open(PATH_SYSFS_OPERATION, O_RDWR);
	char buff[50];
	int s = sprintf(buff,"%d %d", op, val);
	write(sim_fd, buff, s);
	fsync(sim_fd);
	close(sim_fd);
}

int mtx_to_driver(){
	int sim_fd = open(PATH_SYSFS_DATA, O_RDWR);
	if(sim_fd<0){
		//return handle_error_file_open(PATH_SYSFS_DATA);
		return (-1);
	}
	int i, j,  w, temp;
	char buff[500];
	FILE *fp = fopen(PATH_FILE_MTX, "r");
	if(fp==NULL){
		//return handle_error_file_open(PATH_FILE_MTX);
		return (-1);
	}
	int ini=0;
	memset(buff, ' ', sizeof(buff));
	for(i=0; i<(N_COLUMN-1)*(N_ROW-1)*2; i++){
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
		driver_set_operation(PATH_SYSFS_OPERATION, OP_MTX, i);
		write(sim_fd, buff, fim-ini+1);
		ini = fim+1;
		fsync(sim_fd);	
	}
	fclose(fp);
	close(sim_fd);
	return(0);
}

int uvz_to_driver(){
	int sim_fd = open(PATH_SYSFS_DATA, O_RDWR);
	if(sim_fd<0){
		return -1;
	}

	char buff[500];
	memset(buff, ' ', sizeof(buff));
	FILE *fp = fopen(PATH_FILE_UVZ, "r");
	if(fp == NULL){
		return -1;
	}
	fseek (fp , 0L , SEEK_END);
	int fim = ftell(fp);
	fseek (fp , 0L , SEEK_SET);
	if(fread(buff, 1, fim, fp)<fim){
		return handle_error_bad_file(PATH_SYSFS_DATA);

	}
	buff[fim]=0;
	driver_set_operation(PATH_SYSFS_OPERATION, OP_POINT, 0);
	write(sim_fd, buff, fim);
	fsync(sim_fd);	
	fclose(fp);
	close(sim_fd);
	return(0);
}

int calibrated_to_driver(){
	int sim_fd = open(PATH_SYSFS_DATA, O_RDWR);
	if(sim_fd<0){
		return (-1);
	}
	char buff[2];
	sprintf(buff, "%d", 1);
	buff[1]=0;
	driver_set_operation(PATH_SYSFS_OPERATION, OP_CALIBRATED, 0);
	fsync(sim_fd);	
	write(sim_fd, buff, 2);
	fsync(sim_fd);	
	return (0);

}

int files_to_driver(){
	int retval = 0;
	retval = mtx_to_driver();
	if(retval)return retval;
	retval=uvz_to_driver();
	if(retval)return retval;
	retval = calibrated_to_driver();	
	return retval;
}

int check_driver_sysfs(){
	if(access(PATH_SYSFS_DATA, W_OK)!=0){
		return(-1);
	}
	if(access(PATH_SYSFS_OPERATION, W_OK)!=0){
		return(-1);
	}
	if(access(PATH_SYSFS_POINT, R_OK)!=0){
		return(-1);
	}
	return (0);
}

int check_files(){
	if(access(PATH_FILE_MTX, R_OK)<0 || access(PATH_FILE_UVZ, R_OK)<0 ){
			return (-1);
		}
	return(0);
}

static void file_backup(char *src){
	struct stat stStat;
	char dst[strlen(src)+5];
	char ext[] = ".bkp";
	strcpy(dst, src);
	strcat(dst, ext);
	int errcode = stat ("uv", &stStat);
	if(errcode==0){ //existe
		
	}
	char cmd[strlen(dst)*2+10];
	sprintf(cmd, "cp %s %s", src, dst);
	system(cmd);
}

static void file_move(char *src1, char *dst1){
	FILE *from = fopen(src1, "r");
	FILE *to = fopen(dst1, "w");
	printf("from %s to %s\n", src1, dst1);
	char ch;
/* copy the file */
  while(!feof(from)) {
	
    ch = fgetc(from);
	 printf("%c", ch);
    if(ferror(from)) {
      printf("Error reading source file.\n");
      exit(1);
    }
    if(!feof(from)) fputc(ch, to);
    if(ferror(to)) {
      printf("Error writing destination file.\n");
      exit(1);
    }
  }
	fclose(from);
	fclose(to);
}

int detect_device(char *out){
	char *device;	
	int i;
	time_t init_time = time(NULL);
	int retval = EXIT_SUCCESS;
	for(i=0;  ;i++){
		device = devices[i];
		if(device==NULL){
			fputc('.', stderr);
			fflush(stderr);
			if(time(NULL)-init_time>10){//10 segundos
				retval = EXIT_FAILURE;
				break;
			}
			i=0;
			continue;
		}else{
			int fd = open(device, O_RDONLY);
			
			if(fd==0){
				printf("FD =0 %s\n", device);
				continue;
			}
			struct timeval tv;
			fd_set set;

			memset((char *)&tv,0,sizeof(tv)); 

			tv.tv_sec = 0;
			tv.tv_usec = 100000; /*100ms*/
			FD_ZERO(&set);
			FD_SET(fd, &set);
			int retval = select(fd+1,&set,NULL,NULL,&tv); 
			if(retval==-1){

			}else if(retval){
				strcpy(out, device);
				retval = EXIT_SUCCESS;
				break;
				
			}else{
				
			}
			
		}
	}
	fputc('\n', stderr);
	return retval;
}

static void file_swap(char *src1, char *src2){
	int len1 = strlen(src1);
	char name_tmp[len1+10];
	strcpy(name_tmp, src1);
	strcat(name_tmp, ".tmp");
	char cmd[len1*2+10];
	file_move(src1, name_tmp);
	file_move(src2, src1);
	file_move(name_tmp, src2);
	remove(name_tmp);
}



int main3(void){
	uvz_to_driver();
	mtx_to_driver();
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui_gtk.h"
#include "unistd.h"
#include "io.h"
#include <errno.h>

int verbose=0;

static void usage(char* cmd){
    fprintf(stderr, "Usage: %s [-h|--help] [-v|--verbose] [-s|--start  <device | >] [-t|--stop] [-c|--calib]\n", cmd);
    fprintf(stderr, "\t-h, --help: print this help message\n");
    fprintf(stderr, "\t-v, --verbose: print debug messages during the process\n");
    fprintf(stderr, "\t-s, --start: manually start the device driver\n");
    fprintf(stderr, "\t-p, --stop: manually detaches the device driver\n");
    fprintf(stderr, "\t-c, --calib: calibration of tablet\n");
}

int main(int argc, char **argv){
	int i;
	int start=0, stop=0, calib=0, verbose=0;
	int need_device, use=0;
	char device_name[50];
	int device=0;
	
	for(i=1; i<argc; i++){
		if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0){
			usage(argv[0]);
			exit(0);
		}
		else if(strcmp("--s", argv[i])==0||strcmp("--start", argv[i])==0){
			start=1;
			if(argc>i+1 && argv[i+1][0]!='-'){
				strcpy(device_name,argv[++i]);//verificar erro
				device=1;
			}
		}
		else if(strcmp(argv[i], "-t")==0 ||	strcmp(argv[i], "--stop")==0){
			stop=1;
		}
		else if(strcmp("--c", argv[i])==0||strcmp("--calib", argv[i])==0){
			calib=1;
		}
		else if((strcmp("--verbose", argv[i])==0)||
					(strcmp("-v", argv[i])==0)){
			verbose=1;
		}
		else{
			usage(argv[0]);
			exit(-1);
		}
	}

	//chamada bloqueante - verificar
	if(start==1){
		if(device==0){//verificar se arquivo contém o device	
			FILE *fp = fopen(PATH_FILE_DEVICE, "r");
			if(fp==NULL){
				fprintf(stderr, "--device needs a device name\n");
				exit(-1);
			}
			fscanf(fp, "%s", device_name);
		}
	/*	char **arg = (char **)malloc(sizeof(char *)*3);
		int i;

		for(i=0; i<3; i++)
			arg[i] = (char *)malloc(sizeof(char)*20);
		
		strcpy(arg[0], "inputattach");
		strcpy(arg[1], "--brtablet");
		strcpy(arg[2], device_name);*/


		system("modprobe brtablet-driver");
		
		char buff[50];
		sprintf(buff, "/etc/brtablet/brtablet-attach %s &", device_name);
		puts(buff);
		system(buff);
		usleep(200000);		

		//verifica se arquivos foram criados

		if(access(PATH_SYSFS_DATA, W_OK)!=0){
			printf("%s\n\n",(char*)strerror(errno));
			exit(-1);
		}
		if(access(PATH_SYSFS_OPERATION, W_OK)!=0){
			printf("%s\n\n",(char*)strerror(errno));
			exit(-1);
		}
		if(access(PATH_SYSFS_POINT, R_OK)!=0){
			printf("%s\n\n",(char*)strerror(errno));
			exit(-1);
		}




		//super("/etc/brtablet/brtablet-input");
		//inputattach(3, arg);
		/*for(i=0; i<3; i++)
			free(arg[i]);
		free(arg);*/

		fprintf(stderr, "Driver Connected\n");
		if(access(PATH_FILE_MTX, R_OK)<0 || access(PATH_FILE_UVZ, R_OK)<0 ){
			fprintf(stderr, "Device need calibration, run %s --calib\n", argv[0]);
			exit(-1);
		}

		mtx_to_driver();
		uvz_to_driver();

	}
	if(stop==1){
<<<<<<< HEAD
		system("modprobe -r brtablet-driver");
=======
		printf("STOP");
>>>>>>> a4f4dd425a838954a47fe9418349be445ffb9bce
	}

	if(calib==1){
		//testa se o driver está levantado
		main_gtk(argc, argv);
		mtx_to_driver();
		uvz_to_driver();
	}

	return (0);
}

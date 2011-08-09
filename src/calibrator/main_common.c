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
	if(argc==1){
		usage(argv[0]);
		exit(0);
	}
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
	if(getuid()!=0){
		fprintf(stderr, "--Operation not permitted; you need to be root\n");		
		exit(-1);
	}
	//chamada bloqueante - verificar
	if(start==1){
		if(access(PATH_SYSFS_DATA, W_OK)==0){//se driver já esta rodando
			fprintf(stderr, "driver is stared, run --stop before init \n");
			exit(-1);
		}
		if(device==0){//se nao passou o device, lê do arquivo
			FILE *fp = fopen(PATH_FILE_DEVICE, "r");
			if(fp==NULL){
				fprintf(stderr, "--device needs a device name\n");
				exit(-1);
			}
			fscanf(fp, "%s", device_name);
		}else{//se passou, grava o nome no arquivo
			FILE *fp = fopen(PATH_FILE_DEVICE, "w");
			if(fp==NULL){
				fprintf(stderr, "%s %s\n\n",PATH_FILE_DEVICE,(char*)strerror(errno));
				exit(-1);
			}
			fprintf(fp, "%s", device_name);
			fclose(fp);		
		}

		if(system("modprobe brtablet-driver")!=0){//levanta o driver, se já foi levantado ele não faz nada
			fprintf(stderr, "Driver don't load");	
			exit(-1);
		}else{
			fprintf(stderr, "Driver load");	
		}
		
		char buff[70];
		if(access(PATH_EXEC_ATTACH, X_OK)!=0){
			fprintf(stderr,"%s %s\n\n",PATH_EXEC_ATTACH,(char*)strerror(errno));
			exit(-1);
		}
		sprintf(buff, "/etc/brtablet/brtablet-attach %s &", device_name);
		puts(buff);
		
		system(buff);
		
		usleep(200000);		

		//verifica se arquivos foram criados
		if(check_driver()<0){
			exit(-1);	
		}
		fprintf(stderr, "Driver Connected\n");
		
		if(check_files()<0){
			exit(-1);
		}

		driver_calibration();

	}
	if(stop==1){
		system("killall brtablet-attach");
		system("modprobe brtablet-driver -r");//retirar depois dos testes
	}

	if(calib==1){
		if(access(PATH_SYSFS_POINT, R_OK)!=0){//se driver já esta rodando
			fprintf(stderr, "driver not connected, run  %s --start\n", argv[0]);
			exit(-1);
		}
		main_gtk(argc, argv);
		driver_calibration();
		
	}

	return (0);
}

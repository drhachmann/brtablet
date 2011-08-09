#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui_gtk.h"
#include "unistd.h"
#include "io.h"
#include "util.h"
#include <errno.h>

int verbose=0;

static void usage(char* cmd){
    fprintf(stderr, "Usage: %s [-h|--help] [-v|--verbose] [-s|--start  <device | >] [-t|--stop] [-c|--calib] [-d|--detect\n", cmd);
    fprintf(stderr, "\t-h, --help: print this help message\n");
    fprintf(stderr, "\t-v, --verbose: print debug messages during the process\n");
    fprintf(stderr, "\t-s, --start: manually start the device driver\n");
    fprintf(stderr, "\t-p, --stop: manually detaches the device driver\n");
    fprintf(stderr, "\t-c, --calib: calibration of tablet\n");
	 fprintf(stderr, "\t-d, --detect: auto detect the file descriptor of device\n");
}

void msg_color_UPPER(FILE* fp, char *color, char *msg){
	int len = strlen(msg)+2;
	int i;
	fprintf_color(fp, color, "\n");
	for(i=0; i<len; i++){
		fprintf_color(fp, color, "*");
	}
	fprintf_color(fp, color, "\n*%s*\n", msg);
	for(i=0; i<len; i++){
		fprintf_color(fp, color, "*");
	}
	fprintf_color(fp, color, "\n\n");
}

int main(int argc, char **argv){
	int i;
	int start=0, stop=0, calib=0, verbose=0, detect=0;
	int need_device, use=0;
	char device_name[50];
	int device=0;

	if(argc==1){
		usage(argv[0]);
		exit(EXIT_SUCCESS);
	}
	for(i=1; i<argc; i++){
		if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0){
			usage(argv[0]);
			exit(EXIT_SUCCESS);
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
		else if((strcmp("--detect", argv[i])==0)||
					(strcmp("-d", argv[i])==0)){
			detect=1;
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
		fprintf_color(stderr, COLOR_WARNING,"--Operation not permitted; you need to be root\n");		
		exit(-1);
	}

	if(start==1){
		if(access(PATH_SYSFS_DATA, W_OK)==0){//se driver já esta rodando
			fprintf_color(stderr, COLOR_WARNING,"driver is already running\n");
			exit(-1);
		}
	}


	if(detect==1){//auto detect device

		fprintf_color(stdout, COLOR_WARNING, "Move the stylus on the touch...\n"); 
		if(detect_device(device_name)==0){
			device=1;
			fprintf_color(stderr, COLOR_OK,"Device detected: OK\n");
		}else{
			fprintf_color(stderr, COLOR_ERROR, "Device detected: FAILURE\n");
			fprintf_color(stderr, COLOR_WARNING, "Continue...\n");
		}
	}

	if(getuid()!=0){
		fprintf_color(stderr, COLOR_WARNING, "--Permission denied. You need to be root\n");	
		msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
		exit(-1);
	}

	//chamada bloqueante - verificar
	if(start==1){
		if(access(PATH_SYSFS_DATA, W_OK)==0){//se driver já esta rodando
			fprintf_color(stderr, COLOR_WARNING,"driver is already running\n");
			exit(-1);
		}
		if(device==0){//se nao passou o device, lê do arquivo
			FILE *fp = fopen(PATH_FILE_DEVICE, "r");
			if(fp==NULL){
				fprintf_color(stderr, COLOR_ERROR, "Read Device name from file :FAILURE\n");
				fprintf_color(stderr, COLOR_WARNING, "--device needs a device name\n");
				msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
				exit(EXIT_FAILURE);
			}else{
				fprintf_color(stderr, COLOR_OK, "Read Device name from file: OK\n");
				fscanf(fp, "%s", device_name);
			}
		}else{//se passou, grava o nome no arquivo
			FILE *fp = fopen(PATH_FILE_DEVICE, "w");
			if(fp==NULL){
				fprintf_color(stderr, COLOR_ERROR, "Write Device name to file: FAILUED\n");
				msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
				fprintf(stderr, "%s %s\n\n",PATH_FILE_DEVICE,(char*)strerror(errno));
				exit(-1);
			}else{
				fprintf(fp, "%s", device_name);
				fprintf_color(stderr, COLOR_OK, "Write Device name to file: OK\n");
				fclose(fp);		
			}
		}
		fprintf_color(stderr, COLOR_OK, "Device %s: OK\n", device_name);
		if(system("modprobe brtablet-driver")==0){//levanta o driver, se já foi levantado ele não faz nada
			fprintf_color(stderr, COLOR_OK,"Driver Insert: OK\n");				
		}else{
			fprintf_color(stderr, COLOR_ERROR, "Driver Insert: FAILED\n");
			msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
			exit(EXIT_FAILURE);
		}
		
		char buff[70];
		if(access(PATH_EXEC_ATTACH, X_OK)!=0){
			fprintf_color(stderr, COLOR_ERROR, "Check%s : FAILURE\n", PATH_EXEC_ATTACH);
			fprintf_color(stderr, COLOR_WARNING, (char*)strerror(errno));
			msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
			exit(EXIT_FAILURE);
		}else{
			fprintf_color(stderr, COLOR_OK, "Exec %s : OK\n", PATH_EXEC_ATTACH);
		}

		sprintf(buff, "/etc/brtablet/brtablet-attach %s &", device_name);
		system(buff);
		usleep(200000);		
		if(access(PATH_LOCK_ATTACH, R_OK)!=0){ //se nao criou, entao bloqueou
			fprintf_color(stderr, COLOR_OK, "Input Attach: OK\n");
		}else{
			fprintf_color(stderr, COLOR_ERROR,"Input Attach: FAILED\n");
			msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
			exit(-1);
		}
		//verifica se arquivos foram criados
		if(check_driver()==0){
			fprintf_color(stderr, COLOR_OK ,"Check SYSFS Files: OK\n");
		}else{
			fprintf_color(stderr, COLOR_ERROR,"Check SYSFS Files: FAILED\n");
			msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
			exit(EXIT_FAILURE);
		}

		
		if(check_files()==0){
			fprintf_color(stderr, COLOR_OK, "Check Files Calibrate: OK\n");
		}else{
			fprintf_color(stderr, COLOR_ERROR, "Check Files Calibrate: FAILED\n");
			fprintf_color(stderr, COLOR_WARNING, "Run %s --calib\n", argv[0]);
			msg_color_UPPER(stderr, COLOR_ERROR,"DRIVER IS NOT RUNNING");
			exit(EXIT_FAILURE);
		}

		files_to_driver();
		msg_color_UPPER(stderr, COLOR_OK, "DRIVER IS RUNNING");

	}
	if(stop==1){
		FILE *fp = popen("ps x | grep brtablet-attach | grep -v color | grep -v grep","r");
		int id=-1;
		char buff[100];
		while (fgets(buff,sizeof(buff),fp)){
			sscanf(buff, "%d", &id);
			char cmd[50];
			sprintf(cmd,"kill %d\n", id);
			system(cmd);
		}
		pclose(fp);
		if(id==-1){
			fprintf_color(stderr, COLOR_WARNING,"Driver is not running\n");
			exit(EXIT_FAILURE);
		}
		msg_color_UPPER(stderr, COLOR_OK,"DRIVER STOP");
		
		system("modprobe brtablet-driver -r");//retirar depois dos testes
	}

	if(calib==1){
		if(access(PATH_SYSFS_POINT, R_OK)!=0){//se driver já esta rodando
			fprintf_color(stderr, COLOR_ERROR, "Driver Connect: FAILED\n");
			fprintf_color(stderr, COLOR_WARNING, "Run %s --start\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		main_gtk(argc, argv);
		if(check_files()==0){
			fprintf(stderr, "Files Calibrate: OK\n");
		}else{
			fprintf(stderr, "Files Calibrate: FAILED, type %s --calib\n", argv[0]);
		}

		files_to_driver();
		
	}

	return (0);
}

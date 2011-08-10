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
    fprintf(stderr, "\t-p, --stop: manually stop the device driver\n");
    fprintf(stderr, "\t-c, --calib: calibration of device\n");
	 fprintf(stderr, "\t-d, --detect: auto detect device\n");
}

static void br_start(char *device_name ){
	if(strcmp(device_name, "")==0){//se nao passou o device, lê do arquivo

	br_msg(MSG_WARNING, "Device needs a device name\n");
	br_msg(MSG_NOTE, "looking for device name on file %s...\n", PATH_FILE_DEVICE);
	FILE *fp = fopen(PATH_FILE_DEVICE, "r");
	if(fp==NULL){				
		br_perror(NULL);
				br_msg(MSG_FAILURE, "Device not found\n");
				exit(EXIT_FAILURE);
			}else{
				if(fscanf(fp, "%s", device_name)==1)
					br_msg(MSG_SUCCESS, "Device found\n");
				else{
					br_msg(MSG_FAILURE, "Device not found\n");
					exit(EXIT_FAILURE);
				}
				fclose(fp);
			}
		}else{//se passou, grava o nome no arquivo
			FILE *fp = fopen(PATH_FILE_DEVICE, "w");
			br_msg(MSG_NOTE, "Writing the device name on file %s\n", PATH_FILE_DEVICE);
			if(fp==NULL){		
				br_msg(MSG_FAILURE, "Device is not written\n");
				br_perror(NULL);
			}else{
				if(fprintf(fp, "%s", device_name)==1){
					br_msg(MSG_FAILURE, "Device written\n");
				}else{
					br_msg(MSG_FAILURE, "Device is not written\n");
				}
				fclose(fp);		
			}
		}
		br_msg(MSG_NOTE, "Device name %s\n", device_name);
		if(system("modprobe brtablet-driver")==0){//levanta o driver, se já foi levantado ele não faz nada
			br_msg(MSG_SUCCESS, "Driver inserted\n");
		}else{
			br_msg(MSG_SUCCESS, "Driver is not inserted\n");
			exit(EXIT_FAILURE);
		}
		
		char buff[70];
		if(access(PATH_EXEC_ATTACH, X_OK)!=0){
			br_perror(NULL);
			br_msg(MSG_FAILURE, "Check %s\n", PATH_EXEC_ATTACH);
			exit(EXIT_FAILURE);
		}else{
			br_msg(MSG_SUCCESS, "Check %s\n", PATH_EXEC_ATTACH);
		}

		sprintf(buff, "/etc/brtablet/brtablet-attach %s &", device_name);
		if(system(buff)==0){
			br_msg(MSG_SUCCESS, "brtablet-attach process is running\n");
		}else{
			br_msg(MSG_FAILURE, "brtablet-attach process is not running\n");
		}
		usleep(200000);		
		if(access(PATH_LOCK_ATTACH, R_OK)!=0){ //se nao criou, entao bloqueou
			br_msg(MSG_SUCCESS, "Input Attached\n");
		}else{
			br_msg(MSG_FAILURE, "Input not Attached\n");
			exit(EXIT_FAILURE);
		}
		//verifica se arquivos foram criados
		
		if(check_driver_sysfs()==0){
			br_msg(MSG_SUCCESS, "SYSFS File OK\n");
		}else{
			br_perror(NULL);
			br_msg(MSG_FAILURE, "SYSFS File ERROR\n");
			exit(EXIT_FAILURE);
		}

		int retval = EXIT_SUCCESS;
		if(check_files()==0){
			br_msg(MSG_SUCCESS, "Device is Calibrated\n");
		}else{
			retval = EXIT_FAILURE;
			br_msg(MSG_WARNING, "Device is not Calibrated\n");
			br_msg(MSG_WARNING,"Run %s --calib\n", "brcalib");
		}
		if(retval==EXIT_SUCCESS){
			if(files_to_driver()==0){
				br_msg(MSG_SUCCESS, "files sent to the driver\n");
			}else{
				perror(NULL);
				br_msg(MSG_FAILURE, "files are not sent to the driver\n");
			}
		}
}

static void br_detect(char *device_name){
	br_msg(MSG_WARNING, "Move the stylus on the touch...\n"); 
	if(detect_device(device_name)==0){
		br_msg(MSG_SUCCESS,"Device Detected\n");
	}else{
		br_msg(MSG_FAILURE,"Device it not Detected\n");
		exit(EXIT_FAILURE);
	}
}

static void br_stop(){
	if(access(PATH_SYSFS_DATA, W_OK)!=0){//se driver não estiver rodando
			br_msg(MSG_WARNING,"driver is not running\n");
			exit(EXIT_FAILURE);
		}
		FILE *fp = popen("ps x | grep brtablet-attach | grep -v color | grep -v grep","r");
		int id=-1;
		char buff[100];
		while (fgets(buff,sizeof(buff),fp)){
			sscanf(buff, "%d", &id);
			char cmd[50];
			sprintf(cmd,"kill %d\n", id);
			if(system(cmd)==0){
				br_msg(MSG_SUCCESS, "Kill %d\n", id);
			}else{
				perror(NULL);
				br_msg(MSG_FAILURE, "Kill %d\n", id);
			}
		}
		pclose(fp);
		if(id==-1){
			br_msg(COLOR_WARNING,"Driver is not running\n");
			exit(EXIT_FAILURE);
		}
		
		
		if(system("modprobe brtablet-driver -r")==0){//retirar depois dos testes
			br_msg(MSG_SUCCESS, "modprore OK\n");
		}else{
			br_msg(MSG_FAILURE, "modprore FAILED\n");
		}

		br_msg(MSG_BOX_SUCCESS, "DRIVER STOP");	
	
}

static void br_calib(){
	if(access(PATH_SYSFS_POINT, R_OK)!=0){//se driver n  está rodando
		br_msg(MSG_FAILURE, "Driver is not connected\n");
		br_msg(MSG_WARNING,  "Run %s --start\n", "brtablet");
		exit(EXIT_FAILURE);
	}
	main_gtk(0, NULL);
	if(files_to_driver()==0){
		br_msg(MSG_SUCCESS, "Files sent to the driver\n");
	}else{
		perror(NULL);
		br_msg(MSG_SUCCESS, "files are not sent to the driver\n");
	}
}

int main(int argc, char **argv){
	int verbose=0;
	int start=0;
	int stop=0;
	int calib=0;
	int detect=0;
	char device_name[50] = "";
	int i;
	int device=0; //retirar
	if(argc==1){
		usage(argv[0]);
		exit(EXIT_SUCCESS);
	}
	for(i=1; i<argc; i++){
		
		if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0){
			usage(argv[0]);
			exit(EXIT_SUCCESS);
		}
		else if(strcmp("--s", argv[i])==0 || strcmp("--start", argv[i])==0){
			start=1;
			if(argc>i+1 && argv[i+1][0]!='-'){
				strcpy(device_name,argv[++i]);//verificar erro
			}
		}
		else if(strcmp(argv[i], "-t")==0 ||	strcmp(argv[i], "--stop")==0){
			stop=1;
		}
		else if(strcmp("--c", argv[i])==0 || strcmp("--calib", argv[i])==0){
			calib=1;
		}
		else if(strcmp("--detect", argv[i])==0 || strcmp("-d", argv[i])==0){
			detect=1;
		}
		else if(strcmp("--verbose", argv[i])==0 || strcmp("-v", argv[i])==0){
			verbose=1;
		}
		else{
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if(getuid()!=0){//se não for root
		br_msg(MSG_FAILURE,"Permission permitted; you need to be root\n");		
		exit(EXIT_FAILURE);
	}

	if(start==1){
		if(access(PATH_SYSFS_DATA, W_OK)==0){//se driver já estiver rodando
			br_msg(MSG_WARNING,"driver is already running\n");
			exit(EXIT_FAILURE);
		}
	}


	if(detect==1){//auto detect device
		br_detect(device_name);
	}

	if(start==1){
		br_start(device_name);
		br_msg(MSG_BOX_SUCCESS, "DRIVER IS RUNNING");					

	}
	if(stop==1){
		br_stop();
	}
		

	if(calib==1){
		br_calib();
	}
		

		

	return (0);
}

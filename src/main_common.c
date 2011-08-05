#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui/gui_gtk.h"

int verbose=0;

static void usage(char* cmd)
{
    fprintf(stderr, "Usage: %s [-h|--help] [-v|--verbose] [--start  <device>] [--stop] [--calib]\n", cmd);
    fprintf(stderr, "\t-h, --help: print this help message\n");
    fprintf(stderr, "\t-v, --verbose: print debug messages during the process\n");
    fprintf(stderr, "\t--start: manually attach the device driver\n");
    fprintf(stderr, "\t--stop: manually detaches the device driver\n");
    fprintf(stderr, "\t--calib: calibration of tablet\n");
}

int main(int argc, char **argv){
//int main_common(int argc, char **argv){
	int i;
	int init=0, stop=0, calib=0, undo=0;
	int need_device, use=0;
	char *device=NULL;
	if(argc==1){
		if(!use)usage(argv[0]);
		use=1;
		exit(-1);
	}
	for(i=1; i<argc; i++){
		if((strcmp(argv[i], "-h")==0)||
			(strcmp(argv[i], "--help")==0)){
				if(!use)usage(argv[0]);
				use=1;
			}
		else if(strcmp("--start", argv[i])==0){
			init=1;
			if(i+1!=argc)
				device = argv[++i];//verificar erro
		}
		else if(strcmp("--stop", argv[i])==0){
			stop=1;
		}
		else if(strcmp("--calib", argv[i])==0){
			calib=1;
		}
		else if((strcmp("--verbose", argv[i])==0)||
					(strcmp("-v", argv[i])==0)){
			verbose=1;
		}
		else{
			if(!use)usage(argv[0]);
			use=1;
		}
	
	}

	//chamada bloqueante - verificar
	if(init==1){

		char **arg = (char **)malloc(sizeof(char *)*3);
		int i;
		for(i=0; i<3; i++)
			arg[i] = (char *)malloc(sizeof(char)*20);
		
		strcpy(arg[0], "inputattach");
		strcpy(arg[1], "--brtablet");
		if(device!=NULL){
			if(verbose)	printf("inputattach --brtablet %s\n", device);
			strcpy(arg[2], device);
			inputattach(3, arg);
		}else{
						if(verbose)	printf("inputattach --brtablet\n");
			inputattach(2, arg);
		}
		
		for(i=0; i<3; i++)
			free(arg[i]);

		free(arg);
	}
	if(stop==1){

	}

	if(calib==1){
		main_gtk(argc, argv);
		mtx_to_driver();
		uvz_to_driver();
	}

	





	return (0);
}

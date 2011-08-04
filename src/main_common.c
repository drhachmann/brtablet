#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui/gui_gtk.h"

int main2(int argc, char **argv){
	//main_common(argc, argv);
}

int main(int argc, char **argv){
//int main_common(int argc, char **argv){
	int i;
	int init=0, stop=0, calib=0, undo=0;
	char *device;
	for(i=1; i!=argc; i++){
		if(strcmp("--init", argv[i])==0){
			init=1;
			device = argv[++i];//verificar erro
			puts(device);
		}
		if(strcmp("--stop", argv[i])==0){
			stop=1;
		}
		if(strcmp("--calib", argv[i])==0){
			calib=1;
		}
		if(strcmp("--undo", argv[i])==0){//desfazer
			undo=1;
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
		strcpy(arg[2], device);

		inputattach(3, arg);
	
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

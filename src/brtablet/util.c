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
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"



void vfprintf_color(FILE* file, char* color, char *format, va_list args){
	fprintf(file, "%s", color);
	vfprintf (file, format , args);
	fprintf(file, "%s", COLOR_NORMAL);
}


void fprintf_color(FILE *file, char *color, char *format, ...){
	va_list args;
	fprintf(file, "%s", color);
	va_start (args, format);
	vfprintf(file, format, args);
	va_end (args);
	fprintf(file, "%s", COLOR_NORMAL);

}




int br_msg(int msg, char *format, ...){
	va_list args;
	va_start (args, format);
	if(msg==MSG_SUCCESS){//error
		fprintf_color(stderr, COLOR_SUCCESS, "SUCESS : ");
		vfprintf_color(stderr, COLOR_SUCCESS, format, args);
	}
	else if(msg==MSG_WARNING){//error
		fprintf_color(stderr, COLOR_WARNING, "WARNING : ");
		vfprintf_color(stderr, COLOR_WARNING, format, args);
	}
	else if(msg==MSG_NOTE){//error
		vfprintf_color(stderr, COLOR_NOTE, format, args);
	}
	else if(msg==MSG_FAILURE){
		fprintf_color(stderr, COLOR_FAILURE, "FAILURE : ");
		vfprintf_color(stderr, COLOR_FAILURE, format, args);
	}else if(msg==MSG_BOX_SUCCESS || msg==MSG_BOX_FAILURE){
		char *color;
		if(msg==MSG_BOX_SUCCESS)color=COLOR_SUCCESS;
		else color=COLOR_FAILURE;
		char text[200];
		vsprintf(text, format, args);
		int len = strlen(text)+6;
		int i;
		fputc('\n', stderr);
		for(i=0; i<len; i++){
			fprintf_color(stderr, color, "*");
		}
		fputc('\n', stderr);
		fprintf_color(stderr, color, "** ");
		vfprintf_color(stderr, color, format, text);
		fprintf_color(stderr, color, " **");
		fputc('\n', stderr);
		for(i=0; i<len; i++){
			fprintf_color(stderr, color, "*");
		}
		fprintf_color(stderr, color, "\n\n");
	}
	va_end (args);
}

void br_perror(char *str){
	if(str==NULL)
		fprintf_color(stderr, COLOR_PERROR, "%s", (char*)strerror(errno));
	else
		fprintf_color(stderr, COLOR_PERROR, "%s: %s",str, (char*)strerror(errno));
}

int handle_error_file_open(char *path){
	if(IDIOM==ENGLISH)
		printf("Error: Can't Open %s\n", path);
	else if(IDIOM==PORTUGUESE)
		printf("Error: foi possível abrir %s\n", path);
	printf("%s\n\n",(char*)strerror(errno));
	exit(-1);
}



int handle_error_bad_file(char *path){
	if(IDIOM==ENGLISH)
		printf("Error: Bad File %s\n", path);
	else if(IDIOM==PORTUGUESE)
		printf("Erro: Arquivo incorreto : %s \n", path);
	printf("%s\n\n",(char*)strerror(errno));
	puts("");
	exit(-1);
}

void msg_error(char *msg){
	printf("Error: %s\n", msg);
	puts("");
}

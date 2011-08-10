#ifndef _UTIL_H_
#define _UTIL_H_

#define ENGLISH 1
#define PORTUGUESE 2

#define IDIOM ENGLISH

#define COLOR_FAILURE "\033[01;31m"
#define COLOR_SUCCESS "\033[01;34m"
#define COLOR_WARNING "\033[01;33m"
#define COLOR_NOTE "\033[01;37m"
#define COLOR_NORMAL "\033[01;37m"

#define COLOR_ERROR "\033[01;37m"
#define COLOR_PERROR "\033[01;37m"
#define COLOR_OK "\033[01;37m"

#define RETURN_SUCCESS 0 
#define RETURN_FAILURE -1 
#define RETURN_WARNING 0xFFFF
#define RETURN_NOTE 0xFFF7

#define MSG_SUCCESS 0
#define MSG_FAILURE 1
#define MSG_WARNING 2
#define MSG_NOTE 3
#define MSG_BOX_SUCCESS 4
#define MSG_BOX_FAILURE 5

typedef struct Point{
	int x;
	int y;
	int z;
}Point;


void message_error_file_open(char *path);
void message_error_bad_file(char *path);

#endif

#ifndef _UTIL_H_
#define _UTIL_H_

#define ENGLISH 1
#define PORTUGUESE 2

#define IDIOM ENGLISH

#define WHITE "\033[00;37m"
#define LIGHT_BLUE "\033[01;34m"
#define LIGHT_RED "\033[01;31m"
#define LIGHT_YELLOW "\033[01;33m"

#define COLOR_ERROR "\033[01;31m"
#define COLOR_OK "\033[01;34m"
#define COLOR_WARNING "\033[01;33m"



#define fprintf_color(file, color, format, args...){\
	fprintf(file, "%s", color);\
	fprintf (file, format , ##args);\
	fprintf(file, "%s", WHITE);\
}

typedef struct Point{
	int x;
	int y;
	int z;
}Point;


void message_error_file_open(char *path);
void message_error_bad_file(char *path);

#endif

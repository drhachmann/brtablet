#ifndef _UTIL_H_
#define _UTIL_H_

#define ENGLISH 1
#define PORTUGUESE 2

#define IDIOM ENGLISH

typedef struct Point{
	int x;
	int y;
	int z;
}Point;


void message_error_file_open(char *path);
void message_error_bad_file(char *path);

#endif

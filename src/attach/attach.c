/*
 * $Id: inputattach.c,v 1.24 2006/02/08 12:19:31 vojtech Exp $
 *
 *  Copyright (c) 2011 Diego Rodrigo Hachmann
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *
 *  Sponsored by SuSE
 *
 *  Twiddler support Copyright (c) 2001 Arndt Schoenewald
 *  Sponsored by Quelltext AG (http://www.quelltext-ag.de), Dortmund, Germany
 */

/*
 * Input line discipline attach program
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */

#include <linux/serio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>


#define PATH_FILE_DEVICE "/etc/brtablet/device"

#define PATH_LOCK_ATTACH "/etc/brtablet/lock_attack"

static int readchar(int fd, unsigned char *c, int timeout)
{
	struct timeval tv;
	fd_set set;

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000; /*100ms*/

	FD_ZERO(&set);
	FD_SET(fd, &set);

	if (!select(fd + 1, &set, NULL, NULL, &tv))
		return -1;

	if (read(fd, c, 1) != 1)
		return -1;

	return 0;
}

static void setline(int fd, int flags, int speed)
{
	struct termios t;

	tcgetattr(fd, &t);

	t.c_cflag = flags | CREAD | HUPCL | CLOCAL;
	t.c_iflag = IGNBRK | IGNPAR;
	t.c_oflag = 0;
	t.c_lflag = 0;
	t.c_cc[VMIN ] = 1;
	t.c_cc[VTIME] = 0;

	cfsetispeed(&t, speed);
	cfsetospeed(&t, speed);

	tcsetattr(fd, TCSANOW, &t);
}

static int logitech_command(int fd, char *c)
{
	int i;
	unsigned char d;

	for (i = 0; c[i]; i++) {
		write(fd, c + i, 1);
		if (readchar(fd, &d, 1000))
			return -1;
		if (c[i] != d)
			return -1;
	}
	return 0;
}

static int magellan_init(int fd, unsigned long *id, unsigned long *extra)
{
	write(fd, "m3\rpBB\rz\r", 9);
	return 0;
}

static int warrior_init(int fd, unsigned long *id, unsigned long *extra)
{
	if (logitech_command(fd, "*S"))
		return -1;

	setline(fd, CS8, B4800);
	return 0;
}

static int spaceball_waitchar(int fd, unsigned char c, unsigned char *d,
				int timeout)
{
	unsigned char b = 0;

	while (!readchar(fd, &b, timeout)) {
		if (b == 0x0a)
			continue;
		*d++ = b;
		if (b == c)
			break;
	}

	*d = 0;

	return -(b != c);
}

static int spaceball_waitcmd(int fd, char c, char *d)
{
	int i;

	for (i = 0; i < 8; i++) {
		if (spaceball_waitchar(fd, 0x0d, d, 1000))
			return -1;
		if (d[0] == c)
			return 0;
	}

	return -1;
}

static int spaceball_cmd(int fd, char *c, char *d)
{
	int i;

	for (i = 0; c[i]; i++)
		write(fd, c + i, 1);
	write(fd, "\r", 1);

	i = spaceball_waitcmd(fd, toupper(c[0]), d);

	return i;
}

#define SPACEBALL_1003		1
#define SPACEBALL_2003B		3
#define SPACEBALL_2003C		4
#define SPACEBALL_3003C		7
#define SPACEBALL_4000FLX	8
#define SPACEBALL_4000FLX_L	9

static int spaceball_init(int fd, unsigned long *id, unsigned long *extra)
{
	char r[64];

	if (spaceball_waitchar(fd, 0x11, r, 4000) ||
	    spaceball_waitchar(fd, 0x0d, r, 1000))
		return -1;

	if (spaceball_waitcmd(fd, '@', r))
		return -1;

	if (strncmp("@1 Spaceball alive", r, 18))
		return -1;

	if (spaceball_waitcmd(fd, '@', r))
		return -1;

	if (spaceball_cmd(fd, "hm", r))
		return -1;

	if (!strncmp("Hm2003B", r, 7))
		*id = SPACEBALL_2003B;
	if (!strncmp("Hm2003C", r, 7))
		*id = SPACEBALL_2003C;
	if (!strncmp("Hm3003C", r, 7))
		*id = SPACEBALL_3003C;

	if (!strncmp("HvFirmware", r, 10)) {

		if (spaceball_cmd(fd, "\"", r))
			return -1;

		if (strncmp("\"1 Spaceball 4000 FLX", r, 21))
			return -1;

		if (spaceball_waitcmd(fd, '"', r))
			return -1;

		if (strstr(r, " L "))
			*id = SPACEBALL_4000FLX_L;
		else
			*id = SPACEBALL_4000FLX;

		if (spaceball_waitcmd(fd, '"', r))
			return -1;

		if (spaceball_cmd(fd, "YS", r))
			return -1;

		if (spaceball_cmd(fd, "M", r))
			return -1;

		return 0;
	}

	if (spaceball_cmd(fd, "P@A@A", r) ||
	    spaceball_cmd(fd, "FT@", r)   ||
	    spaceball_cmd(fd, "MSS", r))
		return -1;

	return 0;
}

static int stinger_init(int fd, unsigned long *id, unsigned long *extra)
{
	int i;
	unsigned char c;
	unsigned char *response = "\r\n0600520058C272";

	if (write(fd, " E5E5", 5) != 5)		/* Enable command */
		return -1;

	for (i = 0; i < 16; i++)		/* Check for Stinger */
		if (readchar(fd, &c, 200) || c != response[i])
			return -1;

	return 0;
}

static int mzp_init(int fd, unsigned long *id, unsigned long *extra)
{
	if (logitech_command(fd, "*X*q"))
		return -1;

	setline(fd, CS8, B9600);
	return 0;
}

static int newton_init(int fd, unsigned long *id, unsigned long *extra)
{
	int i;
	unsigned char c;
	unsigned char response[35] = {
		0x16, 0x10, 0x02, 0x64, 0x5f, 0x69, 0x64, 0x00,
		0x00, 0x00, 0x0c, 0x6b, 0x79, 0x62, 0x64, 0x61,
		0x70, 0x70, 0x6c, 0x00, 0x00, 0x00, 0x01, 0x6e,
		0x6f, 0x66, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x10,
		0x03, 0xdd, 0xe7
	};

	for (i = 0; i < sizeof(response); i++)
		if (readchar(fd, &c, 400) || c != response[i])
			return -1;

	return 0;
}

static int twiddler_init(int fd, unsigned long *id, unsigned long *extra)
{
	unsigned char c[10];
	int count, line;

	/* Turn DTR off, otherwise the Twiddler won't send any data. */
	if (ioctl(fd, TIOCMGET, &line))
		return -1;
	line &= ~TIOCM_DTR;
	if (ioctl(fd, TIOCMSET, &line))
		return -1;

	/*
	 * Check whether the device on the serial line is the Twiddler.
	 *
	 * The Twiddler sends data packets of 5 bytes which have the following
	 * properties: the MSB is 0 on the first and 1 on all other bytes, and
	 * the high order nibble of the last byte is always 0x8.
	 *
	 * We read and check two of those 5 byte packets to be sure that we
	 * are indeed talking to a Twiddler.
	 */

	/* Read at most 5 bytes until we find one with the MSB set to 0 */
	for (count = 0; count < 5; count++) {
		if (readchar(fd, c, 500))
			return -1;
		if ((c[0] & 0x80) == 0)
			break;
	}

	if (count == 5) {
		/* Could not find header byte in data stream */
		return -1;
	}

	/* Read remaining 4 bytes plus the full next data packet */
	for (count = 1; count < 10; count++)
		if (readchar(fd, c + count, 500))
			return -1;

	/* Check whether the bytes of both data packets obey the rules */
	for (count = 1; count < 10; count++) {
		if ((count % 5 == 0 && (c[count] & 0x80) != 0x00) ||
		    (count % 5 == 4 && (c[count] & 0xF0) != 0x80) ||
		    (count % 5 != 0 && (c[count] & 0x80) != 0x80)) {
			/* Invalid byte in data packet */
			return -1;
		}
	}

	return 0;
}

static int fujitsu_init(int fd, unsigned long *id, unsigned long *extra)
{
	unsigned char cmd, data;

	/* Wake up the touchscreen */
	cmd = 0xff; /* Dummy data */;
	if (write(fd, &cmd, 1) != 1)
		return -1;

	/* Wait to settle down */
	usleep(100 * 1000); /* 100 ms */

	/* Reset the touchscreen */
	cmd = 0x81; /* Cold reset */
	if (write(fd, &cmd, 1) != 1)
		return -1;

	/* Read ACK */
	if (readchar(fd, &data, 100) || (data & 0xbf) != 0x90)
		return -1;

	/* Read status */
	if (readchar(fd, &data, 100) || data != 0x00)
		return -1;

	return 0;
}

static int dump_init(int fd, unsigned long *id, unsigned long *extra)
{
	unsigned char c, o = 0;

	c = 0x80;

	if (write(fd, &c, 1) != 1)         /* Enable command */
                return -1;

	while (1)
		if (!readchar(fd, &c, 1)) {
			printf("%02x (%c) ", c, ((c > 32) && (c < 127)) ? c : 'x');
			o = 1;
		} else {
			if (o) {
				printf("\n");
				o = 0;
			}
		}
}



struct input_types {
	const char *name;
	const char *name2;
	const char *desc;
	int speed;
	int flags;
	unsigned long type;
	unsigned long id;
	unsigned long extra;
	int flush;
	int (*init)(int fd, unsigned long *id, unsigned long *extra);
};

static struct input_types input_types[] = {

{ /*"--mshack"*/"--brtablet",		/*"-ms"*/"-brt",		"3-button mouse in Microsoft mode",
	/*B1200*/B115200, /*CS7*/CS8,
	SERIO_MS,		/*0x00*/0x01,	/*0x01*/0x00,	/*1*/0,	NULL },
{ NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL }
};

static void show_help(void)
{
	struct input_types *type;

	puts("");
	puts("Usage: inputattach [--daemon] [--baud <baud>] <mode> <device>");
	puts("");
	puts("Modes:");

	for (type = input_types; type->name; type++)
		printf("  %-16s %-8s  %s\n",
			type->name, type->name2, type->desc);

	puts("");
}


int inputattach(int argc, char **argv){
	unsigned long devt;
	
	int ldisc;
	struct input_types *type = NULL;
	char *device = NULL;
	int daemon_mode = 0;
	int need_device = 0;
	unsigned long id, extra;
	int fd;
	int i;
	char c;
	int retval;
	int baud = -1;

	for (i = 1; i < argc; i++) {
		if (!strcasecmp(argv[i], "--help")) {
			show_help();
			return EXIT_SUCCESS;
		} else if (!strcasecmp(argv[i], "--daemon")) {
			daemon_mode = 1;
		} else if (need_device) {
			device = argv[i];
			need_device = 0;
		} else if (!strcasecmp(argv[i], "--baud")) {
			if (argc <= i + 1) {
				show_help();
				fprintf(stderr,
					"inputattach: require baud rate\n");
				return EXIT_FAILURE;
			}

			baud = atoi(argv[++i]);
		} else {
			if (type && type->name) {
				fprintf(stderr,
					"inputattach: '%s' - "
					"only one mode allowed\n", argv[i]);
				return EXIT_FAILURE;
			}
			for (type = input_types; type->name; type++) {
				if (!strcasecmp(argv[i], type->name) ||
				    !strcasecmp(argv[i], type->name2)) {
					break;
				}
			}
			if (!type->name) {
				fprintf(stderr,
					"inputattach: invalid mode '%s'\n",
					argv[i]);
				return EXIT_FAILURE;
			}
			need_device = 1;
		}
	}

	if (!type || !type->name) {
		fprintf(stderr, "inputattach: must specify mode\n");
		return EXIT_FAILURE;
        }

	if (need_device) {
		fprintf(stderr, "inputattach: must specify device\n");
		return EXIT_FAILURE;
	}

	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "inputattach: '%s' - %s\n",
			device, strerror(errno));
		return 1;
	}

	switch(baud) {
	case -1: break;
	case 2400: type->speed = B2400; break;
	case 4800: type->speed = B4800; break;
	case 9600: type->speed = B9600; break;
	case 19200: type->speed = B19200; break;
	case 38400: type->speed = B38400; break;
	case 115200: type->speed = B115200; break;
	default:
		fprintf(stderr, "inputattach: invalid baud rate '%d'\n",
				baud);
		return EXIT_FAILURE;
	}
	//return(0);//AKIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
	setline(fd, type->flags, type->speed);


	/*acho que é aki o bloqueio*/
	if (type->flush)
		while (!readchar(fd, &c, 100))
			/* empty */;

	id = type->id;
	extra = type->extra;

	if (type->init && type->init(fd, &id, &extra)) {
		fprintf(stderr, "inputattach: device initialization failed\n");
		return EXIT_FAILURE;
	}

	ldisc = N_MOUSE;
	if (ioctl(fd, TIOCSETD, &ldisc)) {
		fprintf(stderr, "inputattach: can't set line discipline\n");
		return EXIT_FAILURE;
	}

	devt = type->type | (id << 8) | (extra << 16);

	if (ioctl(fd, SPIOCSTYPE, &devt)) {
		fprintf(stderr, "inputattach: can't set device type\n");
		return EXIT_FAILURE;
	}

	retval = EXIT_SUCCESS;
	if (daemon_mode && daemon(0, 0) < 0) {
		perror("inputattach");
		retval = EXIT_FAILURE;
	}

	if(read(fd, NULL, 0)<0){
		
	}

	ldisc = 0;
	ioctl(fd, TIOCSETD, &ldisc);/*ou aki*/
	close(fd);
	return retval;
}


int main(int argc, char **argv){

	char msg[100];
	char **arg = (char **)malloc(sizeof(char *)*3);
	int i;

	for(i=0; i<3; i++)
		arg[i] = (char *)malloc(sizeof(char)*50);
	
	strcpy(arg[0], "inputattach");
	strcpy(arg[1], "--brtablet");
	strcpy(arg[2], argv[1]);

	if(access(PATH_LOCK_ATTACH, R_OK)==0){
		sprintf(msg, "rm %s", PATH_LOCK_ATTACH);
		if(system(msg)<0){
			perror(msg);
		}
	}
	
	if(inputattach(3, arg)==EXIT_FAILURE){
		sprintf(msg, "touch %s", PATH_LOCK_ATTACH);
		if(system(msg)<0){
			perror(msg);
		}
	}
	

	for(i=0; i<3; i++)
		free(arg[i]);
	free(arg);
	return (0);
}

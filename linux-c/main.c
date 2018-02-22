/*
 * M18ST05 VFD Display Tool
 *
 * (c) 2018 Nils Stec <stecdose@gmail.com>
 *
 *
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_help();
void init_serial(char *port);
int send232(char *data, int len);

/* display functions */
void goto_line(int line);
void text_line0(char *s);
void text_line1(char *s);

void clock_set();

int fd_sp;
struct termios spset;

int main(int argc, char **argv) {
	int itemp;
	
	if(argc < 3) {
		print_help(argv[0]);
		exit(-1);
	}

	init_serial(argv[1]);

	if(!strcmp(argv[2], "clear")) {
		send232(" \x1B\x22\x1BP\x1B\x21\x1BP", 8);
	} else if(!strcmp(argv[2], "clear_cur")) {
		send232("\x1BP", 2);
	} else if(!strcmp(argv[2], "goto_line")) {
		itemp = strtoul(argv[3], NULL, 10);
		goto_line(itemp);
	} else if(!strcmp(argv[2], "cr")) {
		send232("\x1BQ", 2);
	} else if(!strcmp(argv[2], "text")) {
		send232(argv[3], strlen(argv[3]));
	} else if(!strcmp(argv[2], "textl0")) {
		text_line0(argv[3]);
	} else if(!strcmp(argv[2], "textl1")) {
		text_line1(argv[3]);
	} else if(!strcmp(argv[2], "clock_on")) {
		send232("\x1B\x05", 2);
	} else if(!strcmp(argv[2], "clock_move")) {
		send232("\x1B\x04", 2);
	} else if(!strcmp(argv[2], "clock_normal")) {
		send232("\x1B\x03", 2);
	} else if(!strcmp(argv[2], "clock_set")) {
		clock_set();
		send232("\x1B\x03", 2);	// clock not moving
		send232("\x1B\x05", 2);	// clock on
	} else {
		printf("'%s' is not understood!\n", argv[2]);
	}

	close(fd_sp);


	return 0;

}

void clock_set() {
	char wrbuf[32];
	char hour, minute, day, mon;
	short year;
// 	time_t current_time;
// 
// 	current_time = time(NULL);
	
	hour = 01;
	minute = 02;
	day = 03;
	mon = 04;
	year = 5678;
	
	sprintf(wrbuf, "%02x02%x02%x02%x04%x", minute, hour, day, mon, year);
	
	
// 	wrbuf[1] = 0;
	send232("\x1B\x00\x10\x10", 4);
	send232(wrbuf, 16);
	
	return;
}

void goto_line(int line) {
	switch(line) {
		case 0:
			send232("\x1B\x21", 2);
			break;
		case 1:
			send232("\x1B\x22", 2);
			break;
		default:	// default line 0
			send232("\x1B\x21", 2);
			break;
	}
	return;
}

void text_line0(char *s) {
	int len = strlen(s);
	if(len > 16) len = 16;
	goto_line(0);
	send232(s, len);
	return;
}

void text_line1(char *s) {
	int len = strlen(s);
	if(len > 16) len = 16;
	goto_line(1);
	send232(s, len);
	return;
}

int send232(char *data, int len) {
	int written = write(fd_sp, data, len);
	
	if(written != len) {
		printf("error while sending data to display!\n");
	}
	
	return written;
}

void print_help(char *own) {
	printf("M18ST05A VFD TOOL\n");	
	printf("(c) 2018 Nils Stec <stecdose@gmail.com> <github.com/spacerace/M18ST05A>\n");
	printf("\n");
	printf("command format:\n");
	printf(" %s <port> <command> <data>\n", own);
	printf("these commands are available:\n");
	printf(" %s <port> clear                         clear text display\n", own);
	printf(" %s <port> clear_cur                     clear current line\n\n", own);
	printf(" %s <port> goto_line 0/1                 set cursor to 1st/2nd line\n", own);
	printf(" %s <port> cr                            send CR\n\n", own);
	printf(" %s <port> text <text>                   text at cursor\n", own);
	printf(" %s <port> textl0 <text>                 text on line 0\n", own);
	printf(" %s <port> textl1 <text>                 text on line 1\n\n", own);
	printf(" %s <port> clock_on                      show clock\n", own);
	printf(" %s <port> clock_move / clock_normal     moving/not moving clock\n", own);
	printf(" %s <port> clock_set                     set clock from system time\n", own);

	return;
}

void init_serial(char *port) {
	// setup a serial port for communication with display
	fd_sp = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd_sp == -1) {
		printf("error while opening '%s'!\nexit.\n", port);
		exit(-2);
	}

	tcgetattr(fd_sp, &spset);

	spset.c_cflag &= ~PARENB;		// no parity
	spset.c_cflag &= ~CSTOPB;		// one stopbit
	spset.c_cflag &= ~CSIZE;
	spset.c_cflag |= CS8;			// eight databits

	spset.c_cflag &= ~CRTSCTS;		// ho hardware flow control
	spset.c_cflag |= (CREAD | CLOCAL);	// turn on receiver

	spset.c_iflag &= ~(IXON|IXOFF|IXANY);	// no software flow control
	spset.c_iflag &= ~(ICANON|ECHO|ECHOE|ISIG);	// non canonical mode

	spset.c_oflag &= ~OPOST;

	if((tcsetattr(fd_sp, TCSANOW, &spset) != 0)) {
		printf("error while setting termios config for '%s'!\nexit.\n", port);
		close(fd_sp);
		exit(-3);
	}

	cfsetispeed(&spset, B9600); /* Set Read  Speed as 9600                       */
	cfsetospeed(&spset, B9600); /* Set Write Speed as 9600                       */
}

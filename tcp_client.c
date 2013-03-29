/*
   client of the TCP
   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include <linux/in.h>

#define PORT 8888  	/* the listen port */
#define MAXN 1024

//struct  sockaddr_in {
//	u8 sin_len;
//	u8 sin_family;
//	u16 sin_port;
//	struct in_addr sin_addr;
//	char sin_zero[8];
//};
//
//struct in_addr {
//	u32 s_addr; 	/* 32 bit IP address */
//};

static int s;
void process_conn_client(int);

int main(int argc, char *argv[])
{

	struct sockaddr_in server_addr; 	/* server addr struct */
	
	int err; 	/* error flag */

	/* create a socket */
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) { /* error */
		printf ("socket error\n");
		return -1;
	}

	/* set the addr */
	bzero (&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 		/* protocol family */
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// #define	INADDR_ANY		((unsigned long int) 0x00000000)

	server_addr.sin_port = htons(PORT); 	/* server port */
	
	/* ip address to int */
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	/* connect the server */
	connect (s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));

	process_conn_client(s);

	close(s);
}

void process_conn_client(int s)
{
	ssize_t size = 0; // signed size_t(unsigned int)  == typedef int ssize_T

	char buffer[MAXN];
	
	for (;;)
	{
		size = read (0, buffer, MAXN); // read data from stdin
		if (size > 0)
		{
			write (s, buffer, size); 	// send to server
			size = read (s, buffer, MAXN); // read from server
			write (1, buffer, size); // write to the stdout
		}
	}
}

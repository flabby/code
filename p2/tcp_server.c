/*
   server of the TCP
   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include <linux/in.h>
#define PORT 8888  	/* the listen port */
#define BACKLOG 2 	/* the queue length of listen*/

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

void process_conn_server(int s);

int main(int argc, char *argv[])
{
	int ss;		/* server socket */
	int sc; 	/* client socket */

	struct sockaddr_in server_addr; 	/* server addr struct */
	struct sockaddr_in client_addr; 	/* client addr struct */
	
	int err; 	/* error flag */
	pid_t pid; 	/* id of new process */

	/* create a socket */
	ss = socket(AF_INET, SOCK_STREAM, 0);
	if (ss < 0) { /* error */
		printf ("socket error\n");
		return -1;
	}

	/* set the addr */
	bzero (&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 		/* protocol family */
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// #define	INADDR_ANY		((unsigned long int) 0x00000000)

	server_addr.sin_port = htons(PORT); 	/* server port */
	
	/* bind struct to socket descriptor */
	err = bind(ss, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err < 0)
	{
		printf ("bind error\n");
		return -1;
	}

	/* listen */
	err = listen(ss, BACKLOG);
	if (err < 0)
	{
		 printf ("listen error\n");
		 return -1;
	}

	/* main loop */
	for (;;)
	{
		int addrlen = sizeof(struct sockaddr);
		/* accept the client connections */
		sc = accept (ss, (struct sockaddr*)&client_addr, &addrlen);
		if (sc < 0)
		{
			continue; 
		}

		/* fork a new process to handle the request */
		pid = fork();
		if (pid == 0) 	// son process
		{
			close(ss);
			process_conn_server(sc); // handle
		}
		else
		{
			close(sc); 		// father process close the connection
		}
	}
}

void process_conn_server(int s)
{
	ssize_t size = 0;
	char buffer[MAXN];

	for (;;)
	{
		size = read(s, buffer, MAXN);
		if (size == 0) // no data
			return ;
		
		sprintf (buffer, "%d bytes altogether\n", size);
		write (s, buffer, strlen(buffer) + 1);
	}
}


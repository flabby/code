/*
   server  pthread: 1 for connections; 1 for request
   */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>

#define BUFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
#define CLIENTNUM 1024 	// the max number of  client supported

// file descriptor of clients
int connect_host[CLIENTNUM];
int connect_num = 0;

static void *handle_request(void *argv)
{
	time_t now;
	char buf[BUFLEN];
	int n = 0;

	int maxfd = -1; 	// max listened file desctriptors
	fd_set scanfd;		// set of file descriptors
	struct timeval timeout; 	// delay
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int i = 0;
	int err = -1;

	while (1)
	{
		maxfd = -1;				// init the maxfd
		FD_ZERO (&scanfd);		// clear the set

		for (i = 0; i < CLIENTNUM; i++)
		{
			if (connect_host[i] != -1)
			{
				FD_SET (connect_host[i], &scanfd); 	//insert to set
				if (maxfd < connect_host[i])
					maxfd = connect_host[i];
			}
		}

		// select wait
		err = select (maxfd + 1, &scanfd, NULL, NULL, &timeout);
		switch (err)
		{
			case 0: // time out
			case -1:// error
				break;
			default:
				if (connect_num <= 0)
					break;

				for (i = 0; i < CLIENTNUM; i++)
				{
					if (connect_host[i] != -1 
							&& FD_ISSET(connect_host[i], &scanfd))
					{
						memset (buf, 0, sizeof(buf));
						n = recv(connect_host[i], buf, BUFLEN, 0);
						if (n > 0 && !strncmp (buf, "TIME", 4))
						{
							memset (buf, 0, sizeof(buf));
							now = time(NULL);
							sprintf (buf, "%24s\r\n", ctime(&now));

							send (connect_host[i], buf, strlen(buf), 0);
						}

						connect_host[i] = -1;
						connect_num--;

						close (connect_host[i]);
					}
				}
				break;
		}
	}
	return 0;
}
						
static void *handle_connect(void *argv)
{
	int s_s = *((int *)argv); // server
	int s_c = -1; 	// client
	
	struct sockaddr_in from;
	int len = sizeof(from);

	while (1)
	{
		int i = 0;
		s_c	= accept (s_s, (struct sockaddr*)&from, &len);

		printf ("a client connect, from:%s\n", inet_ntoa(from.sin_addr));

		for (i = 0; i < CLIENTNUM; i++)
		{
			if (connect_host[i] == -1)
			{
				connect_host[i] = s_c;
				connect_num++;
				break;
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int s_s; 	// server socket
	struct sockaddr_in local;

	int i = 0; 
	
	memset (connect_host, -1, sizeof(connect_host));

	s_s = socket (AF_INET, SOCK_STREAM, 0);
	if (s_s < 0)
	{
		printf ("sock error!\n");
		return -1;
	}

	// init the sockaddr
	memset (&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);

	// bind the sockaddr to local address and port
	int err = bind (s_s, (struct sockaddr*)&local, sizeof(local));
	if (err < 0)
	{
		printf ("bind error\n");
		return -1;
	}

	err = listen (s_s, BACKLOG);
	if (err < 0)
	{
		printf ("listen error\n");
		return -1;
	}

	pthread_t thread_do[2];

	pthread_create (&thread_do[0], // id of thread
			NULL, // property
			handle_connect, // call back function
			(void *)&s_s);	// parameters

	// thread for request
	pthread_create(&thread_do[1],
			NULL,
			handle_request,
			NULL);

	for (i = 0; i < 2; i++)
		pthread_join (thread_do[i], NULL);

	close (s_s);

	return 0;
}



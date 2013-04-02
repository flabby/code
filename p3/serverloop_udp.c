/*
   loop server of UDP
   */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define BUFLEN 1024
#define SERVER_PORT 8888

int main(int argc, char *argv[])
{
	int s; 		// server socket
	struct sockaddr_in local, to; 	//local address
	time_t now;
	char buf[BUFLEN];	//buffer
	int n = 0;
	int len = sizeof(to);

	// create UDP socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		printf ("socket error!\n");
		return -1;
	}

	// initialize he address */
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT); 	

	// bind the socket to local address and port
	int err = bind(s, (struct sockaddr*)&local, sizeof(local));
	if (err < 0)
	{
		printf ("bind error!\n");
		return -1;
	}

	while (1)
	{
		memset(buf, 0, sizeof(buf));
		n = recvfrom(s,  buf, BUFLEN, 0, (struct sockaddr*)&to, &len);
		if (n > 0 && !strncmp(buf, "TIME", 4))
		{
			memset(buf, 0, sizeof(buf));
			now = time(NULL);	// present time
			sprintf (buf, "%24s\r\n", ctime(&now));
			sendto (s, buf, strlen(buf), 0, (struct sockaddr*)&to, len);
		}
	}
	close(s);

	return 0;
}

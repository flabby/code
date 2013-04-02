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
	int s;	// server socket
	struct sockaddr_in server;

	time_t now;
	char buf[BUFLEN];
	int n = 0;
	int len = 0;

	s = socket (AF_INET, SOCK_DGRAM, 0);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	memset (buf, 0, sizeof(buf));
	strcpy (buf, "TIME");

	sendto(s, buf, strlen(buf), 0, (struct sockaddr*)&server, sizeof(server));
	
	memset (buf, 0, sizeof(buf));
	len = sizeof(server);
	n = recvfrom (s, buf, BUFLEN, 0, (struct sockaddr*)&server, &len);
	if (n > 0)
	{
		printf ("TIME: %s\n", buf);
	}
	close (s);

	return 0;
}

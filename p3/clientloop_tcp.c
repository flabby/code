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
	char buf[BUFLEN];
	int n = 0;

	s = socket (AF_INET, SOCK_STREAM, 0);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	int err = connect (s, (struct sockaddr*)&server, sizeof(server));
	memset (buf, 0, sizeof(buf));
	strcpy (buf, "TIME");

	send(s, buf, strlen(buf), 0);
	
	memset (buf, 0, sizeof(buf));
	n = recv (s, buf, BUFLEN, 0);
	if (n > 0)
	{
		printf ("TIME: %s\n", buf);
	}
	close (s);

	return 0;
}

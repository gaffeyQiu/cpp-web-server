#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

const char* SERVER_HOST = "127.0.0.1";
const int SERVER_PORT = 8080;

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket()");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
	serv_addr.sin_port = htons(SERVER_PORT);

	if ( connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0 ) {
		perror("connect()");
		exit(1);
	}
	return 0;
}
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include "util.h"


int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	errif(sockfd == -1, "socket()");

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
	serv_addr.sin_port = htons(SERVER_PORT);
	
	errif(bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) != 0, "bind()");

	errif(listen(sockfd, SOMAXCONN) != 0, "listen()");

	printf("The Web Server Listening %s:%d\n", SERVER_HOST, SERVER_PORT);

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	bzero(&client_addr, sizeof(client_addr));

	int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);

	printf("[client] fd: %d, IP: %s, Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	return 0;
}
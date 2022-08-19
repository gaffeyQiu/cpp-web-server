#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

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


	int epfd = epoll_create1(0);
	errif(epfd == -1, "epoll_create1()");

	struct epoll_event events[MAX_EVENTS], ev;
	bzero(&events, sizeof(events));
	bzero(&ev, sizeof(ev));
	ev.data.fd = sockfd;
	ev.events = EPOLLIN | EPOLLET;
	setnonblocking(sockfd);
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	while (true) {
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		errif(nfds == -1, "epoll_wait()");

		for(int i = 0; i < nfds; i++) {
			if (events[i].data.fd == sockfd) {	// 客户端链接
				struct sockaddr_in client_addr;
				bzero(&client_addr, sizeof(client_addr));
				socklen_t client_addr_len = sizeof(client_addr);

				int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
				errif(client_sockfd == -1, "accept()");
				printf("New Client: fd: %d, IP: %s, Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

				bzero(&ev, sizeof(ev));
				ev.data.fd = client_sockfd;
				ev.events = EPOLLIN | EPOLLET;
				setnonblocking(client_sockfd);
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
			} else if (events[i].events & EPOLLIN) { // 可读事件
				char buf[READ_BUFFER];
				while (true) {
					bzero(&buf, sizeof(buf));
					ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
					if(bytes_read > 0){
                        printf("[client %d]: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
						if (buf == "q") {
							close(events[i].data.fd);
							break;
						}
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
				}
			} else {
				printf("something happened\n");
			}
		}
	}

	close(sockfd);
	return 0;
}
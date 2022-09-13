#pragma once

class InetAddress;
class Socket {
	private:
		int fd;
		int enable = 1;
	public:
		Socket();
		Socket(int);
		~Socket();

		void bind(InetAddress*);
		void listen();
		void setnonblocking();
		int accept(InetAddress*);
		int getFd();
};
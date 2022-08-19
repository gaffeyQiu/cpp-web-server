all: server client

server: clear
	g++ server.cpp -o server
client: clear
	g++ client.cpp -o client
clear:
	rm -rf server client
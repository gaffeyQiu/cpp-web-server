all: server client

server:
	g++ util.cpp server.cpp -o server
client:
	g++ util.cpp client.cpp -o client
clear:
	rm server client *.out
all: server client

server:
	g++ src/*.cpp server.cpp -o server
client:
	g++ src/util.cpp client.cpp -o client
clear:
	rm server client *.out
CPPFLAGS=-g -Wall -Werror -pedantic -std=c++11
LDFLAGS=-g -pthread

all: server client

# Compile server and run
server_compile_run: server server_run

server_run:
	./ipk-simpleftp-server -u ./data/credentials -f ./data/server_working_directory -i eth0 -p 115

server: main_server.o
	g++ $(LDFLAGS) -o ipk-simpleftp-server build/main_server.o build/SFTP_Server.o

main_server.o: src/main_server.cpp SFTP_Server.o
	g++ $(CPPFLAGS) -c src/main_server.cpp -o build/main_server.o

SFTP_Server.o: src/SFTP_Server.cpp inc/SFTP_Server.h
	g++ $(CPPFLAGS) -c src/SFTP_Server.cpp -o build/SFTP_Server.o


client: main_client.o
	g++ $(LDFLAGS) -o ipk-simpleftp-client build/main_client.o build/SFTP_Client.o

main_client.o: src/main_client.cpp SFTP_Client.o
	g++ $(CPPFLAGS) -c src/main_client.cpp -o build/main_client.o

SFTP_Client.o: src/SFTP_Client.cpp inc/SFTP_Client.h
	g++ $(CPPFLAGS) -c src/SFTP_Client.cpp -o build/SFTP_Client.o

clean:
	rm -f xmachy02.tar ipk-simpleftp-client ipk-simpleftp-server build/*

tar:
	tar -cf xmachy02.tar inc/* src/* Makefile Readme.md doc/manual.pdf build/ data/

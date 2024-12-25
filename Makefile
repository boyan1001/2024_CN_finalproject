all:
	-g++ testclient.cpp -o client.o -lm -lpthread -lssl -lcrypto
	-g++ testserver.cpp -o server.o -lm -lpthread -lssl -lcrypto

clean:
	rm -f *.o
	rm -f server
	rm -f client
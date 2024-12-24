all:
	-g++ -pthread -o server.o testserver.cpp
	-g++ -pthread -o client.o testclient.cpp

clean:
	rm -f *.o
	rm -f server
	rm -f client
all:
	-g++ -pthread -o server testserver.cpp
	-g++ -pthread -o client testclient.cpp

clean:
	rm -f *.o
	rm -f server
	rm -f client
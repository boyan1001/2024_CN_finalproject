all:
	-g++ servermain.cpp server.cpp src/UI.cpp -o server.o -lm -lpthread
	-g++ clientmain.cpp client.cpp src/UI.cpp -o client.o -lm -lpthread

clean:
	rm -f *.o
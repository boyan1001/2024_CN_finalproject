all:
	-g++ server.cpp src/UI.cpp -o server.o -lm -lpthread
	-g++ client.cpp src/UI.cpp -o client.o -lm -lpthread

clean:
	rm -f *.o
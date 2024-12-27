all:
	-g++ testclient.cpp -o client.o -lm -lpthread -lssl -lcrypto -lSDL2 -lportaudio
	-g++ testserver.cpp -o server.o -lm -lpthread -lssl -lcrypto -lSDL2 -lportaudio

clean:
	rm -f *.o
	rm -f server
	rm -f client
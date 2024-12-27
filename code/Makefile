all:
	-g++ servermain.cpp src/crypt.cpp src/UI.cpp src/file.cpp src/audio.cpp server.cpp -o server.o -lm -lpthread -lssl -lcrypto -lSDL2
	-g++ clientmain.cpp src/crypt.cpp src/UI.cpp src/file.cpp src/audio.cpp client.cpp -o client.o -lm -lpthread -lssl -lcrypto -lSDL2

clean:
	rm -f *.o
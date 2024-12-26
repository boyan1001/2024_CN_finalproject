all:
	-g++ servermain.cpp src/crypt.cpp src/UI.cpp src/file.cpp server.cpp -o server.o -lm -lpthread -lssl -lcrypto
	-g++ clientmain.cpp src/crypt.cpp src/UI.cpp src/file.cpp client.cpp -o client.o -lm -lpthread -lssl -lcrypto

clean:
	rm -f *.o
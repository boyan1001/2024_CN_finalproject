all:
	-g++ servermain.cpp src/crypt.cpp src/UI.cpp server.cpp -o server.o -lm -lpthread -lssl -lcrypto
	-g++ clientmain.cpp src/crypt.cpp src/UI.cpp client.cpp -o client.o -lm -lpthread -lssl -lcrypto

clean:
	rm -f *.o
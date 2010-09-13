all: tcp server expire

tcp: tcp.c
	gcc -o tcp tcp.c

server: server.o board.o
	g++ -g -o server server.o board.o -lbsd

server.o: server.cpp board.h
	g++ -c -g -o server.o server.cpp

board.o: board.cpp board.h
	g++ -c -DDEBUG -g -o board.o board.cpp

expire: expire.c
	gcc -o expire expire.c

clean:
	rm -f *.o tcp server expire

release:
	rm -f server.tar.gz
	tar zcvf server.tar.gz server.cpp board.h board.cpp expire.c Makefile htdocs
	mv server.tar.gz /var/www/htdocs/benzedrine.cx/planetwars/

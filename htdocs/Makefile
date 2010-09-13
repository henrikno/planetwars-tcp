all: getplayer getratings getmaps canvas

canvas: canvas.c cgi.o
	gcc -static -o canvas canvas.c cgi.o -lz

getplayer: getplayer.cpp cgi.o
	g++ -static -o getplayer getplayer.cpp cgi.o -lz

getmaps: getmaps.cpp cgi.o
	g++ -static -o getmaps getmaps.cpp cgi.o -lz

getratings: getratings.cpp cgi.o
	g++ -static -o getratings getratings.cpp cgi.o -lz

cgi.o: cgi.c cgi.h
	gcc -static -c -o cgi.o cgi.c

clean:
	rm -f canvas getplayer getratings getmaps *.o *.core

install:
	cp canvas canvas.html getplayer getratings getmaps /var/www/htdocs/benzedrine.cx/planetwars/


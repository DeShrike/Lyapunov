CC=gcc
AR=ar

all: lyapunov

rmbin:
	rm lyapunov

clean:
	rm *.o

###################################00

lyapunov: lyapunov.o pngimage.o
	$(CC) -o lyapunov lyapunov.o pngimage.o -lpng -lm

lyapunov.o: lyapunov.c lyapunov.h pngimage.h
	$(CC) -c -O3 lyapunov.c

utils.o: utils.c utils.h
	$(CC) -c -O3 utils.c

pngimage.o: pngimage.c pngimage.h
	$(CC) -c -O3 pngimage.c

###################################00

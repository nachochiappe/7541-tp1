#Makefile - TP1 (Sistema de Votacion)

#Variables:
EXEC=tp
CC=gcc
CFLAGS= -std=c99 -g -Wall -Wconversion -Wtype-limits -pedantic -Werror
OBJECTS=cola.o lectura.o lista.o parser.o pila.o vector_dinamico.o votacion.o
VALGRIND= valgrind --leak-check=full --track-origins=yes

all: $(EXEC)

cola.o: cola.c cola.h
	$(CC) $(CFLAGS) -c cola.c

lectura.o: lectura.c lectura.h
	$(CC) $(CFLAGS) -c lectura.c

lista.o: lista.c lista.h
	$(CC) $(CFLAGS) -c lista.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

pila.o: pila.c pila.h
	$(CC) $(CFLAGS) -c pila.c

vector_dinamico.o: vector_dinamico.c vector_dinamico.h
	$(CC) $(CFLAGS) -c vector_dinamico.c

votacion.o: votacion.c votacion.h
	$(CC) $(CFLAGS) -c votacion.c

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXEC)

valgrind: $(EXEC)
	$(VALGRIND) ./$(EXEC)

clean: 
	rm -f *.o *~

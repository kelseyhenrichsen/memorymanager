# Makefile for sleeping TA
CC=gcc
CFLAGS=-lpthread

all: sleeping_ta

sleeping_ta: main.o student.o ta.o help_student.o programming.o
	$(CC) -o sleeping_ta main.o student.o ta.o help_student.o programming.o $(CFLAGS)

main.o: main.c ta.h
	$(CC) -c main.c $(CFLAGS)

student.o: student.c ta.h
	$(CC) -c student.c $(CFLAGS)

ta.o: ta.c ta.h
	$(CC) -c ta.c $(CFLAGS)

help_student.o: help_student.c ta.h
	$(CC) -c help_student.c $(CFLAGS)

programming.o: programming.c ta.h
	$(CC) $(CFLAGS) -lpthread -c programming.c

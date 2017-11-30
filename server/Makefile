PHONY = clean
LIB = ../lib

CC = gcc
CFLAGS = -g -c -I
OUTPUT = main
OBJFILES = main.c

$(OUTPUT) : $(OBJFILES)
        $(CC) -o $(OUTPUT) $(OBJFILES) -L../lib -I$(INCLUDE) -lip_check
clean:
        rm  server


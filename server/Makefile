.PHONY = clean
INCLUDE = ../include
LIB = ../lib
 
CC = gcc
CFLAGS = -g -c -I
OUTPUT = server
OBJFILES = server_code.c

$(OUTPUT) : $(OBJFILES)
	export LD_LIBRARY_PATH=../lib
	$(CC) -o $(OUTPUT) $(OBJFILES) -L$(LIB) -I$(INCLUDE) -lmyfuncs -pthread
clean:
	rm  server





.PHONY : clean
CC = gcc -fPIC
CFLAGS = -g -I
OUTPUT = libmyfuncs.so.1
OBJFILES = ipcheck.o
INCLUDE = ../include

$(OUTPUT) : $(OBJFILES)
	ld -shared -soname=$(OUTPUT) *.o -o libmyfuncs.so.1.0 -lc
	ln -s libmyfuncs.so.1.0 libmyfuncs.so
	ln -s libmyfuncs.so.1.0 libmyfuncs.so.1
%.o : %.c
	gcc -fPIC -c $^ -I$(INCLUDE)
clean:
	rm *.o $(OUTPUT)
	rm libmyfuncs.so libmyfuncs.so.1.0



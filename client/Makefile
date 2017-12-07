.PHONY = clean 
CC = gcc
CFLAGS = -g 
OUTPUT = client
OBJFILES = client.c

$(OUTPUT) : $(OBJFILES)
	$(CC) -o $(OUTPUT) $(OBJFILES) -pthread
clean:
	rm  client



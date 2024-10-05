CC = gcc
CFLAGS = -Wall -g
TARGET = ssi

all: $(TARGET)

$(TARGET): ssi.o
	$(CC) $(CFLAGS) -o $(TARGET) ssi.o

ssi.o: ssi.c
	$(CC) $(CFLAGS) -c ssi.c

clean:
	rm -f *.o $(TARGET)

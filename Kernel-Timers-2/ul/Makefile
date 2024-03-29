CC=arm-linux-gnueabihf-gcc -static
CFLAGS=-Wall
TARGET=ktimer
SRC=ktimer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

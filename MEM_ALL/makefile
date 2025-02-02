CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic-errors -DNDEBUG

TARGET = main
OBJS   = main.o custom_Allocator.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

custom_Allocator.o: customAllocator.c customAllocator.h
	$(CC) $(CFLAGS) -c customAllocator.c -o $@

main.o: main.c customAllocator.h
	$(CC) $(CFLAGS) -c main.c -o $@

clean:
	rm -f $(OBJS) $(TARGET)
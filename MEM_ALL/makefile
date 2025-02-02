CC=gcc
CFLAGS= -std=c99 -Wall -Werror -pedantic-errors -DNDEBUG

TARGET=main
OBJS=main.o custom_Allocator.o

all: $(TARGET)

$(TARGET): $(OBJS)
$(CC) $(CFLAGS) -o $(OBJS)

custom_Allocator.o: customAllocator.c customAllocator.h
$(CC) $(CFLAGS) -o customAllocator.c

clean: 
rm -f $(OBJS) $(TARGET)

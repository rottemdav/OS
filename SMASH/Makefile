# Compiler and flags
CC = gcc
CFLAGS = -g -Wall

# Target name
TARGET = smash

# Object files
OBJS = smash.o commands.o signals.o jobs.o

# Default rule: Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rules for building object files
smash.o: smash.c commands.h signals.h jobs.h constants.h
	$(CC) $(CFLAGS) -c smash.c

commands.o: commands.c commands.h jobs.h constants.h
	$(CC) $(CFLAGS) -c commands.c

signals.o: signals.c signals.h constants.h
	$(CC) $(CFLAGS) -c signals.c

jobs.o: jobs.c jobs.h constants.h
	$(CC) $(CFLAGS) -c jobs.c

# Cleaning old files before a new build
clean:
	rm -f $(TARGET) $(OBJS) *~ core.*
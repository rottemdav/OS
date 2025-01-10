CXX = g++
CXXFLAGS = -g -Wall

OBJECTS = main.o bank.o atm.o log.o
TARGET = bank

all: $(TARGET)

$(TARGET): $(OBJS)
   $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

bank.o: bank.hpp atm.hpp bank_account.hpp
  $(CXX) $(CXXFLAGS) -o bank.cpp 

atm.o: atm.hpp bank.hpp log.hpp bank_account.hpp
  $(CXX) $(CXXFLAGS) -o atm.cpp 

bank_account.o: read_write.hpp
  $(CXX) $(CXXFLAGS) -o bank_account.cpp 

log.o: log.hpp
  $(CXX) $(CXXFLAGS) -o log.cpp 

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean

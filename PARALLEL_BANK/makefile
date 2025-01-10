CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread

OBJECTS = main.o bank.o atm.o bank_account.o log.o read_write.o 
TARGET = bank

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp bank.hpp atm.hpp log.hpp	
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o 

bank.o: bank.cpp bank.hpp bank_account.hpp atm.hpp read_write.hpp
	$(CXX) $(CXXFLAGS) -c bank.cpp -o bank.o

atm.o: atm.cpp atm.hpp bank.hpp log.hpp bank_account.hpp
	$(CXX) $(CXXFLAGS) -c atm.cpp -o atm.o

bank_account.o: bank_account.cpp read_write.hpp
	$(CXX) $(CXXFLAGS) -c bank_account.cpp -o bank_account.o

log.o: log.cpp log.hpp
	$(CXX) $(CXXFLAGS) -c log.cpp -o log.o

read_write.o: read_write.cpp read_write.hpp
	$(CXX) $(CXXFLAGS) -c read_write.cpp -o read_write.o

clean:
	rm -f $(OBJECTS) $(TARGET)

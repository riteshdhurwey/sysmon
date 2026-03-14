CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = sysmon

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -lncursesw -ltinfo -lpthread

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	sudo cp $(TARGET) /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

clean:
	rm -f src/*.o $(TARGET)
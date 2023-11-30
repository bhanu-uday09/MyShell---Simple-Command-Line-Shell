CC := g++
CXXFLAGS := -std=c++17 -Wall

TARGET := myshell

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

SRC=$(wildcard ./src/*.cpp)
OBJ=$(patsubst ./src/%.cpp, ./obj/%.o, $(SRC))
INCLUDE=./include/
CC=g++
TARGET=Scanner
CXXFLAGS=-w -lpthread -I
ALL:$(TARGET)

$(OBJ):./obj/%.o:./src/%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS) $(INCLUDE)

$(TARGET):$(OBJ)
	$(CC) $(OBJ) -lpthread -o $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ)

run:
	./$(TARGET)
.PHONY:clean ALL run

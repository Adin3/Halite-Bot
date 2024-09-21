# the compiler: gcc for C program, define as g++ for C++
CC = g++
RM = rm
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall
# The build target 
TARGET = MyBot
build: $(TARGET)

$(TARGET): src/$(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) src/$(TARGET).cpp

run: $(TARGET)
	./MyBot
clean:
	rm MyBot
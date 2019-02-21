OBJECTS = ssu_cp.o main.o #ssu_path.o
TARGET = ssu_cp
CC = gcc
CFLAGS =  -g

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

all : $(TARGET)

clean :
	rm $(TARGET)
	rm $(OBJECTS)

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
	rm -f files/*.txt files/accounts/*.txt
	$(MAKE)

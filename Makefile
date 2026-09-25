TARGET = cproject
SRC := $(wildcard src/*.c)

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -std=c11 -Wno-missing-braces -Iinclude

# Libraries and linking
# LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Build rule
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

install:
	sudo cp $(TARGET) /usr/bin
	sudo chmod a+x /usr/bin/$(TARGET)

uninstall:
	sudo rm -rf /usr/bin/$(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)

# Run rule
run: $(TARGET)
	./$(TARGET)

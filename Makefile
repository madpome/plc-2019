# Repertoires du projet

BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
OBJPROF_DIR = obj-prof

# Options de compilation/édition des liens

CC = clang
LD = clang
INC = -I$(INC_DIR)

CFLAGS += $(INC) -Wall -std=c99 -O0 -g  -Wextra
LDFLAGS = 

# Liste des fichiers objet

# les notres...
OBJPROF_FILES =  $(OBJPROF_DIR)/huffman.o  $(OBJPROF_DIR)/jpeg_reader.o $(OBJPROF_DIR)/bitstream.o

# et les votres!
OBJ_FILES = $(OBJ_DIR)/example.o

# cible par défaut

TARGET = $(BIN_DIR)/example

all: $(TARGET)

$(TARGET): $(OBJPROF_FILES) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJPROF_FILES) $(OBJ_FILES) -o $(TARGET)

$(OBJ_DIR)/example.o: $(SRC_DIR)/example.c $(INC_DIR)/jpeg_reader.h $(INC_DIR)/bitstream.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/example.c -o $(OBJ_DIR)/example.o

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJ_FILES)

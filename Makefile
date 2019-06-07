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
OBJPROF_FILES = $(OBJPROF_DIR)/bitstream.o  $(OBJPROF_DIR)/jpeg_reader.o #$(OBJPROF_DIR)/huffman.o

# et les votres!
OBJ_FILES = $(OBJ_DIR)/jpeg2ppm.o $(OBJ_DIR)/color.o $(OBJ_DIR)/decomp.o $(OBJ_DIR)/quant_inv.o $(OBJ_DIR)/zigzag.o $(OBJ_DIR)/idct.o $(OBJ_DIR)/initialisation_image.o $(OBJ_DIR)/mcu.o   $(OBJ_DIR)/huffman.o $(OBJ_DIR)/notre_jpeg_reader.o


# cible par défaut

TARGET = $(BIN_DIR)/jpeg2ppm

INC_FILES = $(INC_DIR)/color.h $(INC_DIR)/decomp.h  $(INC_DIR)/quant_inv.h $(INC_DIR)/zigzag.h $(INC_DIR)/idct.h $(INC_DIR)/huffman.h $(INC_DIR)/jpeg_reader.h $(INC_DIR)/bitstream.h $(INC_DIR)/initialisation_image.h $(INC_DIR)/mcu.h #$(INC_DIR)/notre_jpeg_reader.h

all: $(TARGET)

$(TARGET): $(OBJPROF_FILES) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJPROF_FILES) $(OBJ_FILES) -o $(TARGET) -lm

$(OBJ_DIR)/jpeg2ppm.o: $(SRC_DIR)/jpeg2ppm.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/jpeg2ppm.c -o $(OBJ_DIR)/jpeg2ppm.o

$(OBJ_DIR)/color.o: $(SRC_DIR)/color.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/color.c -o $(OBJ_DIR)/color.o

$(OBJ_DIR)/decomp.o: $(SRC_DIR)/decomp.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/decomp.c -o $(OBJ_DIR)/decomp.o

$(OBJ_DIR)/quant_inv.o: $(SRC_DIR)/quant_inv.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/quant_inv.c -o $(OBJ_DIR)/quant_inv.o

$(OBJ_DIR)/idct.o: $(SRC_DIR)/idct.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/idct.c -o $(OBJ_DIR)/idct.o -lm

$(OBJ_DIR)/zigzag.o: $(SRC_DIR)/zigzag.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/zigzag.c -o $(OBJ_DIR)/zigzag.o

$(OBJ_DIR)/initialisation_image.o: $(SRC_DIR)/initialisation_image.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/initialisation_image.c -o $(OBJ_DIR)/initialisation_image.o

$(OBJ_DIR)/mcu.o: $(SRC_DIR)/mcu.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/mcu.c -o $(OBJ_DIR)/mcu.o

$(OBJ_DIR)/notre_jpeg_reader.o: $(SRC_DIR)/notre_jpeg_reader.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/notre_jpeg_reader.c -o $(OBJ_DIR)/notre_jpeg_reader.o

$(OBJ_DIR)/huffman.o: $(SRC_DIR)/huffman.c $(INC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/huffman.c -o $(OBJ_DIR)/huffman.o

#$(OBJ_DIR)/bitstream.o: $(SRC_DIR)/bitstream.c $(INC_FILES)
#	$(CC) $(CFLAGS) -c $(SRC_DIR)/bitstream.c -o $(OBJ_DIR)/bitstream.o

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJ_FILES)

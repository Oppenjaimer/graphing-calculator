SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
PARSER_DIR = ../math-parser

TARGET = $(BIN_DIR)/plot

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -I$(INC_DIR) -I$(PARSER_DIR)/include
LDFLAGS = -L$(PARSER_DIR)/lib/release -lmathparser -lm -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -g
    BUILD_DIR = build/debug
    BIN_DIR = bin/debug
else
    CFLAGS += -O3 -march=native -flto
    BUILD_DIR = build/release
    BIN_DIR = bin/release
endif

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean

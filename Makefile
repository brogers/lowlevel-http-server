CC = gcc
CFLAGS  = -Wall -Wextra -g -std=c23
CFLAGS += -Iinclude
CFLAGS += -I$(VENDOR_DIR)/cjson

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
VENDOR_DIR = vendor
BIN_DIR = bin

TARGET = $(BIN_DIR)/myhttpd

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

CJSON_SRC = $(VENDOR_DIR)/cjson/cJSON.c
CJSON_OBJ = $(OBJ_DIR)/$(VENDOR_DIR)/cjson/cJSON.o

all: $(TARGET)

$(TARGET): $(OBJ_FILES) $(CJSON_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Vendored cJSON: build without the project warning flags, mirroring the
# CMake setup (vendor/CMakeLists.txt builds it as its own lib).
$(CJSON_OBJ): $(CJSON_SRC)
	mkdir -p $(dir $@)
	$(CC) -I$(VENDOR_DIR)/cjson -g -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

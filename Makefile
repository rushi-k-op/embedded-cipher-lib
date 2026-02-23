# =============================================================================
# Embedded Cipher Library — Makefile
#
# Targets:
#   make            — build the static library (libcipher.a) + demo
#   make test       — build and run the unit test suite
#   make clean      — remove all build artefacts
#
# Cross-compile example (ARM bare-metal):
#   make CC=arm-none-eabi-gcc AR=arm-none-eabi-ar
# =============================================================================

CC      ?= gcc
AR      ?= ar
CFLAGS  := -Wall -Wextra -Wpedantic -std=c99 -Iinclude
ARFLAGS := rcs

# Directories
SRC_DIR   := src
INC_DIR   := include
TEST_DIR  := tests
BUILD_DIR := build

# Targets
LIB       := $(BUILD_DIR)/libcipher.a
DEMO      := $(BUILD_DIR)/demo
TEST_BIN  := $(BUILD_DIR)/test_cipher

# Source files
LIB_SRC   := $(SRC_DIR)/cipher.c
TEST_SRC  := $(TEST_DIR)/test_cipher.c
DEMO_SRC  := $(SRC_DIR)/demo.c

.PHONY: all test clean

all: $(LIB)

# Static library
$(LIB): $(LIB_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/cipher.o
	$(AR) $(ARFLAGS) $@ $(BUILD_DIR)/cipher.o

# Demo executable
$(DEMO): $(DEMO_SRC) $(LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lcipher -o $@

# Test executable + run
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lcipher -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
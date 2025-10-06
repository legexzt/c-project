# Makefile for Password Manager

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = 
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/password_manager
TARGET_WIN = $(BUILD_DIR)/password_manager.exe

# Source files
SOURCES = $(SRC_DIR)/main_terminal.c
SOURCES_WIN = $(SRC_DIR)/main.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target (Linux terminal version)
all: $(TARGET)

# Windows GUI version
windows: $(TARGET_WIN)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile Linux terminal version
$(TARGET): $(SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LIBS)
	@echo "✓ Build complete! Executable: $(TARGET)"

# Compile Windows GUI version
$(TARGET_WIN): $(SOURCES_WIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES_WIN) -o $@ -lcomctl32 -lgdi32
	@echo "✓ Build complete! Executable: $(TARGET_WIN)"

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)/password_manager $(BUILD_DIR)/password_manager.exe $(BUILD_DIR)/*.o
	@echo "✓ Build artifacts cleaned."

# Clean everything including build directory
distclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "All build files removed."

# Run the application
run: $(TARGET)
	@./$(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build Linux terminal version (default)"
	@echo "  windows   - Build Windows GUI version"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all build files"
	@echo "  run       - Build and run the Linux terminal version"
	@echo "  help      - Show this help message"

.PHONY: all windows clean distclean run help

CC = gcc
LD = ld
AS = nasm
OBJCOPY = objcopy
OBJDUMP = objdump

CFLAGS += -Wall -Wextra -g -O3 -z noexecstack
LIB_ARGPARSE = /usr/local/lib/libargparse.a
ASFLAGS = -f elf64 -g

BUILD_DIR = ./build
TARGET_NAME = fastAES
TARGET = $(BUILD_DIR)/$(TARGET_NAME)

C_SOURCES := $(wildcard *.c)
NASM_SOURCES := $(wildcard *.nasm)
C_OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
NASM_OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(NASM_SOURCES:.nasm=.o)))
OBJECTS := $(C_OBJECTS) $(NASM_OBJECTS)

NASM_LISTINGS := $(addprefix $(BUILD_DIR)/, $(notdir $(NASM_SOURCES:.nasm=.L)))
C_ASM_LISTINGS := $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.s)))
C_OBJ_LISTINGS := $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.lst)))


.PHONY: all
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIB_ARGPARSE)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.nasm
	$(AS) $(ASFLAGS) $< -o $@

.PHONY: listings
listings: c_listings nasm_listings

.PHONY: c_listings
c_listings: $(C_ASM_LISTINGS) $(C_OBJ_LISTINGS)

$(BUILD_DIR)/%.s: %.c
	$(CC) $(CFLAGS) -S $< -o $@

$(BUILD_DIR)/%.lst: $(BUILD_DIR)/%.o
	$(OBJDUMP) -S $< > $@

.PHONY: nasm_listings
nasm_listings: $(NASM_LISTINGS)

$(BUILD_DIR)/%.L: %.nasm
	$(AS) $(ASFLAGS) -l $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: rebuild
rebuild: clean all
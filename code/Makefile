TARGET_EXEC ?= code

BUILD_DIR ?= ./build
TARGET_DIR ?= ./bin
SRC_DIRS ?= .

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= $(INC_FLAGS) -MMD -MP -O3 -Wall -Wextra -Wno-unused-parameter -Werror -pedantic -std=c99

.PHONY: all run clean

# Compilation seulement
all: $(TARGET_DIR)/$(TARGET_EXEC)

# Exécution avec argument fichier
run: all
	@clear
	@valgrind --leak-check=full --track-origins=yes ./$(TARGET_DIR)/$(TARGET_EXEC) $(fichier)
	@make clean

# Génération de l'exécutable
$(TARGET_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compilation des .c
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

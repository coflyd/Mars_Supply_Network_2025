# Mars Supply Network - build file
# Usage: make | make run | make debug | make clean

CC      ?= cc
TARGET  := mars
SRC_DIR := src
OBJ_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS  += -std=c11 -Wall -Wextra -O2 -MMD -MP
LDLIBS  += -lSDL2 -lSDL2_ttf -lm

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
    CFLAGS  += -I$(BREW_PREFIX)/include -DGL_SILENCE_DEPRECATION
    LDFLAGS += -L$(BREW_PREFIX)/lib
    LDLIBS  += -framework OpenGL -framework GLUT
else
    CFLAGS  += $(shell pkg-config --cflags sdl2 SDL2_ttf 2>/dev/null)
    LDLIBS  += -lGL -lGLU -lglut
endif

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS := $(filter-out -O2,$(CFLAGS)) -O0 -g -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

-include $(OBJS:.o=.d)

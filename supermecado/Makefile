# ==========================================================================
#  SmartStore - Makefile (Linux, macOS e Windows)
#
#  Alvos:
#    make            compila o projeto (modo release)
#    make run        compila e executa
#    make debug      compila com símbolos de depuração (-g -O0)
#    make rebuild    limpa e compila do zero
#    make clean      remove build/ e bin/
#    make distclean  remove também a raylib baixada em vendor/
#    make help       mostra esta ajuda
#
#  Raylib:
#    Se houver uma raylib instalada no sistema (via pkg-config) ela é usada.
#    Caso contrário, a raylib $(RAYLIB_VERSION) é baixada para vendor/ e
#    compilada automaticamente na primeira execução (requer git).
#
#    Para usar uma raylib já extraída em outro lugar:
#      make RAYLIB_PATH=C:/raylib/raylib-5.5_win64_mingw-w64
#    Para forçar a versão baixada mesmo com raylib no sistema:
#      make RAYLIB_VENDOR=1
# ==========================================================================

CC        := gcc
TARGET    := smartstore

SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build
BIN_DIR   := bin

SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/ui/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

CFLAGS  := -std=c99 -Wall -Wextra -I$(INC_DIR) -MMD -MP
LDFLAGS :=
LDLIBS  :=

RAYLIB_VERSION := 5.5
RAYLIB_REPO    := https://github.com/raysan5/raylib.git
RAYLIB_DIR     := vendor/raylib
RAYLIB_LIB     := $(RAYLIB_DIR)/src/libraylib.a

# --------------------------------------------------------------------------
#  Plataforma
# --------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    PLATAFORMA := windows
    EXE        := .exe
    SYS_LIBS   := -lopengl32 -lgdi32 -lwinmm

    # cmd.exe preserva as aspas no echo; sh (Git Bash/MSYS) não.
    ifeq ($(shell echo "x"),"x")
        WIN_PATH = $(subst /,\,$(1))
        MKDIR    = if not exist "$(call WIN_PATH,$(1))" mkdir "$(call WIN_PATH,$(1))"
        RMDIR    = if exist "$(call WIN_PATH,$(1))" rmdir /S /Q "$(call WIN_PATH,$(1))"
        RUN      = $(call WIN_PATH,$(1))
    endif
else
    UNAME := $(shell uname -s)

    ifeq ($(UNAME),Darwin)
        PLATAFORMA := macos
        SYS_LIBS   := -framework OpenGL -framework Cocoa -framework IOKit \
                      -framework CoreAudio -framework CoreVideo
    else
        PLATAFORMA := linux
        SYS_LIBS   := -lGL -lm -lpthread -ldl -lrt -lX11
    endif

    ifndef RAYLIB_VENDOR
        TEM_PKG_CONFIG := $(shell pkg-config --exists raylib 2>/dev/null && echo sim)
    endif
endif

MKDIR ?= mkdir -p $(1)
RMDIR ?= rm -rf $(1)
RUN   ?= ./$(1)

# --------------------------------------------------------------------------
#  Origem da raylib: RAYLIB_PATH > pkg-config > vendor/
# --------------------------------------------------------------------------
ifdef RAYLIB_PATH
    RAYLIB_ORIGEM := $(RAYLIB_PATH)
    CFLAGS  += -I$(RAYLIB_PATH)/include
    LDFLAGS += -L$(RAYLIB_PATH)/lib
    LDLIBS  += -lraylib $(SYS_LIBS)
else ifeq ($(TEM_PKG_CONFIG),sim)
    RAYLIB_ORIGEM := sistema (pkg-config)
    CFLAGS  += $(shell pkg-config --cflags raylib)
    LDLIBS  += $(shell pkg-config --libs raylib)
else
    RAYLIB_ORIGEM := $(RAYLIB_DIR) (v$(RAYLIB_VERSION))
    USA_VENDOR    := sim
    CFLAGS  += -I$(RAYLIB_DIR)/src
    LDFLAGS += -L$(RAYLIB_DIR)/src
    LDLIBS  += -lraylib $(SYS_LIBS)
endif

BIN := $(BIN_DIR)/$(TARGET)$(EXE)

# --------------------------------------------------------------------------
#  Regras
# --------------------------------------------------------------------------
.PHONY: all release debug run rebuild clean distclean help info

all: release

release: CFLAGS += -O2
release: $(BIN)

debug: CFLAGS += -g -O0 -DDEBUG
debug: $(BIN)

$(BIN): $(OBJS)
	@$(call MKDIR,$(@D))
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(USA_VENDOR),sim)
$(OBJS): | $(RAYLIB_LIB)

$(RAYLIB_DIR)/src/Makefile:
	git clone --depth 1 --branch $(RAYLIB_VERSION) $(RAYLIB_REPO) $(RAYLIB_DIR)

$(RAYLIB_LIB): $(RAYLIB_DIR)/src/Makefile
	$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC CC=$(CC)
endif

run: $(BIN)
	$(call RUN,$(BIN))

rebuild: clean
	@$(MAKE) --no-print-directory all

clean:
	@$(call RMDIR,$(BUILD_DIR))
	@$(call RMDIR,$(BIN_DIR))

distclean: clean
	@$(call RMDIR,vendor)

info:
	@echo "Plataforma: $(PLATAFORMA)"
	@echo "Raylib:     $(RAYLIB_ORIGEM)"
	@echo "Executavel: $(BIN)"

help:
	@echo "make            - compila o projeto"
	@echo "make run        - compila e executa"
	@echo "make debug      - compila com simbolos de depuracao"
	@echo "make rebuild    - limpa e compila do zero"
	@echo "make clean      - remove build/ e bin/"
	@echo "make distclean  - remove tambem a raylib baixada (vendor/)"
	@echo "make info       - mostra plataforma e origem da raylib"

-include $(DEPS)

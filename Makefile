TARGET  = senior-playground
CXX     = g++
CC      = gcc
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc
CFLAGS   = -std=c11   -O2 -Isrc

SRCS_CXX = src/main.cpp src/shader.cpp src/mesh.cpp src/camera.cpp \
           src/texture.cpp src/scene.cpp src/equipment.cpp
SRCS_C   = src/image.c

OBJS = $(SRCS_CXX:src/%.cpp=build/%.o) $(SRCS_C:src/%.c=build/%.o)

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    BREW     := $(shell brew --prefix)
    CXXFLAGS += -DGL_SILENCE_DEPRECATION -I$(BREW)/include
    CFLAGS   += -DGL_SILENCE_DEPRECATION -I$(BREW)/include
    # GLM header-only — point to wherever it is; fall back to FetchContent build
    GLM_INC  := $(wildcard $(BREW)/include/glm $(HOME)/.cmake/packages/glm/*/include)
    ifneq ($(GLM_INC),)
        CXXFLAGS += -I$(firstword $(GLM_INC))/..
    endif
    LDFLAGS  = -L$(BREW)/lib -lGLEW -lglfw -framework OpenGL -lm
else
    LDFLAGS  = -lGL -lGLEW -lglfw -lm
endif

OUTDIR = build

.PHONY: all clean run

all: $(OUTDIR)/$(TARGET) assets shaders

$(OUTDIR)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp | $(OUTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.c | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR):
	mkdir -p $(OUTDIR)

assets: | $(OUTDIR)
	cp -r assets/. $(OUTDIR)/

shaders: | $(OUTDIR)
	mkdir -p $(OUTDIR)/shaders
	cp shaders/* $(OUTDIR)/shaders/

run: all
	cd $(OUTDIR) && ./$(TARGET)

clean:
	rm -rf $(OUTDIR)

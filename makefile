
.PHONY = all web
VPATH = %.cpp src src/objects src/terrain  src/utilities
OBJ_PATH := Debug
WS_PATH := WSDebug

WS_SRCS := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
WS_SRCS := $(notdir $(WS_SRCS))
WS_BINS := $(WS_SRCS:%.cpp=$(WS_PATH)/%.o)

SRCS := $(wildcard src/*.c)
SRCS := $(notdir $(SRCS))
BINS := $(WS_SRCS:%.cpp=$(OBJ_PATH)/%.o) $(SRCS:%.c=$(OBJ_PATH)/%.o)
# BINS := 


# $(info VAR is $(SRCS))
$(info BINS is $(BINS))

EMCC := em++
EMFLAGS := -I include -I . -g -O0 -Wall -std=c++17 -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2
EMLFLAGS := --preload-file font.png --preload-file src/shaders --preload-file texture.png

CC := g++
CCFLAGS := -I include -I . -g -O0 -Wall -std=c++17
LFLAGS :=  -L . -lglfw3 -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl
PROGNAME := $(OBJ_PATH)/voxelgame
WS_PROGNAME := $(WS_PATH)/voxelgame.html

all: $(OBJ_PATH) ${PROGNAME}
web: $(WS_PATH) ${WS_PROGNAME}

${WS_PROGNAME}: ${WS_BINS}
	$(EMCC) $(EMFLAGS) $(EMLFLAGS) $^ -o $@

${WS_PATH}/%.o: %.cpp
	$(EMCC) $(EMFLAGS) -c $< -o $(WS_PATH)/$(@F)

${PROGNAME}: ${BINS}
	$(CC) $^ -lm $(LFLAGS) -o $@

$(OBJ_PATH):
	mkdir $@

$(WS_PATH):
	mkdir $@

$(OBJ_PATH)/%.o: %.cpp
	$(CC) $(CCFLAGS) -c $< -o $(OBJ_PATH)/$(@F)

$(OBJ_PATH)/%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $(OBJ_PATH)/$(@F)
# echo $@ $<

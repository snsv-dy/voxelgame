
.PHONY = all
VPATH = %.cpp src src/objects src/terrain  src/utilities
OBJ_PATH := Debug
SRCS := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)  $(wildcard src/*.c)
SRCS := $(notdir $(SRCS))
BINS := $(SRCS:%.cpp=$(OBJ_PATH)/%.o)
BINS := $(BINS:%.c=$(OBJ_PATH)/%.o)

# $(info VAR is $(SRCS))
$(info BINS is $(BINS))

CC := g++
CCFLAGS := -I include -I . -g -O0 -Wall -std=c++17
LFLAGS :=  -L . -lglfw3 -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl
PROGNAME := $(OBJ_PATH)/voxelgame


all: $(OBJ_PATH) ${PROGNAME}

${PROGNAME}: ${BINS}
	$(CC) $? -lm $(LFLAGS) -o $@

$(OBJ_PATH):
	mkdir $@

$(OBJ_PATH)/%.o: %.cpp
	$(CC) $(CCFLAGS) -c $< -o $(OBJ_PATH)/$(@F)

$(OBJ_PATH)/%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $(OBJ_PATH)/$(@F)
# echo $@ $<

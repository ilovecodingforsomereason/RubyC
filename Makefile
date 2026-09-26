CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I.
LIBS = -lglfw -lGL -lGLU -lm

SRCS = ruby_dung.c \
       timer.c \
       player.c \
       textures.c \
       phys/aabb.c \
       level/tesselator.c \
       level/tile.c \
       level/level.c \
       level/frustum.c \
       level/chunk.c \
       level/level_renderer.c

OBJS = $(SRCS:.c=.o)
TARGET = rubyc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

eval cc main.c -I./include $(pkg-config --libs --cflags raylib) -o simulation
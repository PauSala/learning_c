eval cc main.c -I./include $(pkg-config --libs --cflags raylib) -o tdefense  -Wall -Wextra -Wpedantic -Werror
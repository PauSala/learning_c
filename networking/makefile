CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c99 # -fsanitize=address,undefined
SRCS = src/main.c src/html_res.c src/logger.c src/response_t.c src/errors.c src/handler.c
OBJS = $(SRCS:.c=.o)
OUTDIR = out
TARGET = $(OUTDIR)/http_server

# Convert source files to object files inside 'out/' directory
OBJS := $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRCS))

all: $(TARGET)

# Ensure the 'out' directory exists
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Compile object files into 'out/' directory
$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OUTDIR)

rebuild: clean all

.PHONY: all clean rebuild
CFLAGS = -std=c11 -O3

.PHONY: all clean

all: bin/mimus-tokenize

bin/mimus-tokenize: src/tokenize.c src/array.c src/gguf.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude $^ -o $@

clean:
	rm -rf bin

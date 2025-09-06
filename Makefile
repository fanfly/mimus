CFLAGS = -std=c11 -O3

.PHONY: all clean

all: bin/mimus-tokenize

bin/mimus-tokenize: tools/tokenize.c lib/gguf.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf bin

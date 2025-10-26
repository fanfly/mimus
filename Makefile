CFLAGS = -std=c11 -O3

.PHONY: all clean

all: bin/mimus-tokenize

bin/mimus-tokenize: include/gguf.h include/sequence.h src/gguf.c src/sequence.c src/tokenize.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude $^ -o $@

clean:
	rm -rf bin
CFLAGS = -std=c11 -O3

.PHONY: all clean

all: bin/tokenize

bin/tokenize: tools/tokenize.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf bin

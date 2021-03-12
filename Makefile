DIR = $(shell pwd)
PASTA = $(DIR)/filesystem

all: compile run

end: umount clean

compile:
	gcc filesystem.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o passthrough

run:
	mkdir -p $(PASTA)
	./filesystem $(PASTA)

umount:
	umount -l $(PASTA)

clean:
	rm -rf $(PASTA)
	rm filesystem

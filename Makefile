DIR = $(shell pwd)
FS = $(DIR)/fs

DATA = $(DIR)/fs_data

all: compile run

end: umount clean

compile:
	gcc filesystem.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o filesystem

run:
	mkdir -p $(FS)
	[ -d $(DATA) ] || mkdir -p $(DATA)
	sudo ./filesystem -o allow_other,default_permissions,modules=subdir,subdir=$(DATA) $(FS)

umount:
	sudo umount -l $(FS)

clean:
	rm -rf $(FS)
	rm filesystem

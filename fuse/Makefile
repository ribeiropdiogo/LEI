DIR = $(shell pwd)
FS = $(DIR)/fs

DATA = $(DIR)/fs_data
ROCKS = $(DIR)/rocksdb
POSTGRES = $(DIR)/postgres
TENSOR = $(DIR)/tensorflow

all: compile run

end: umount clean

compile:
	gcc filesystem.c sb.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o filesystem

run:
	mkdir -p $(FS)
	[ -d $(DATA) ] || mkdir -p $(DATA)
	./filesystem -o allow_other,default_permissions,modules=subdir,subdir=$(DATA) $(FS)

umount:
	sudo umount -l $(FS)

clean:
	rm -rf $(FS)
	rm filesystem

rocksdb:
	gcc filesystem.c sb.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o filesystem
	mkdir -p $(ROCKS)
	[ -d $(ROCKS) ] || mkdir -p $(ROCKS)
	./filesystem -o allow_other,default_permissions,modules=subdir,subdir=$(ROCKS) $(FS)

postgres:
	gcc filesystem.c sb.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o filesystem
	mkdir -p $(POSTGRES)
	[ -d $(POSTGRES) ] || mkdir -p $(POSTGRES)
	./filesystem -o allow_other,default_permissions,modules=subdir,subdir=$(POSTGRES) $(FS)

tensorflow:
	gcc filesystem.c sb.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o filesystem
	mkdir -p $(TENSOR)
	[ -d $(TENSOR) ] || mkdir -p $(TENSOR)
	./filesystem -o allow_other,default_permissions,modules=subdir,subdir=$(TENSOR) $(FS)

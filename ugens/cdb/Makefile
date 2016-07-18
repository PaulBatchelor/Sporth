CFLAGS += -I ugens/cdb/

CDB_OBJ = ugens/cdb/cdb_find.o \
ugens/cdb/cdb_findnext.o \
ugens/cdb/cdb_hash.o \
ugens/cdb/cdb_init.o \
ugens/cdb/cdb_make_add.o \
ugens/cdb/cdb_make.o \
ugens/cdb/cdb_make_put.o \
ugens/cdb/cdb_seek.o \
ugens/cdb/cdb_seq.o \
ugens/cdb/cdb_unpack.o 

OBJ += $(CDB_OBJ)

cdb: util/cdb
util/cdb: ugens/cdb/cdb.o $(CDB_OBJ)
	$(CC) $(CFLAGS) $(CDB_OBJ) ugens/cdb/cdb.o -o $@

BIN += util/cdb

CFLAGS += -I ugens/cdb/
CFLAGS += -I ugens/cdb/jsmn

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

CFLAGS += -I ugens/cdb/jsmn/
JSMN_OBJ = ugens/cdb/jsmn/jsmn.o

OBJ += $(CDB_OBJ) $(JSMN_OBJ) 

cdb: util/cdb
util/cdb: ugens/cdb/cdb.o $(CDB_OBJ)
	$(CC) $(CFLAGS) $(CDB_OBJ) ugens/cdb/cdb.o -o $@

cdbjson: util/cdbjson
util/cdbjson: ugens/cdb/cdbjson.o $(CDB_OBJ) $(JSMN_OBJ)
	$(CC) $(CFLAGS) $(CDB_OBJ) $(JSMN_OBJ) ugens/cdb/cdbjson.o -o $@

BIN += util/cdb
BIN += util/cdbjson

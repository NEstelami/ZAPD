CFLAGS = -Wall -O2 -g
UC_OBJ = uc_f3d.o uc_f3db.o uc_f3dex.o uc_f3dexb.o uc_f3dex2.o
OBJ = gfxd.o $(UC_OBJ)
LIB = libgfxd.a

CPPFLAGS-$(MT) += -DCONFIG_MT
CPPFLAGS += $(CPPFLAGS-y)

.PHONY: all
all: $(OBJ) $(LIB)

.PHONY: clean
clean:
	rm -f $(OBJ) $(LIB)

.INTERMEDIATE: $(OBJ)

$(LIB): $(OBJ)
	ar rcsv $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $^

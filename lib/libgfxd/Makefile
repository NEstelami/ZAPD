CFLAGS = -Wall -O2 -g
UC_OBJ = uc.f3d.o uc.f3db.o uc.f3dex.o uc.f3dexb.o uc.f3dex2.o
OBJ = gfxd.o $(UC_OBJ)
LIB = libgfxd.a

CPPFLAGS-$(MT) += -DCONFIG_MT
CPPFLAGS += $(CPPFLAGS-y)

uc.f3d.o: CPPFLAGS += -Duc_name=gfxd_f3d -DF3D_GBI
uc.f3db.o: CPPFLAGS += -Duc_name=gfxd_f3db -DF3D_GBI -DF3D_BETA
uc.f3dex.o: CPPFLAGS += -Duc_name=gfxd_f3dex -DF3DEX_GBI
uc.f3dexb.o: CPPFLAGS += -Duc_name=gfxd_f3dexb -DF3DEX_GBI -DF3D_BETA
uc.f3dex2.o: CPPFLAGS += -Duc_name=gfxd_f3dex2 -DF3DEX_GBI_2

.PHONY: all
all: $(LIB)($(OBJ))

.PHONY: clean
clean:
	rm -f $(OBJ) $(LIB)

.INTERMEDIATE: $(OBJ)

$(OBJ): gbi.h gfxd.h priv.h

$(UC_OBJ): uc.c uc_argfn.c uc_argtbl.c uc_macrofn.c uc_macrotbl.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

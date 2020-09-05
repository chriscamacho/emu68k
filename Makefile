MUSA=support/Musashi
MUSAOBJ=$(MUSA)/m68kcpu.o $(MUSA)/m68kops.o $(MUSA)/softfloat/softfloat.o $(MUSA)/m68kdasm.o
MUSASRC=$(MUSA)/m68kcpu.c $(MUSA)/m68kops.c $(MUSA)/softfloat/softfloat.c $(MUSA)/m68kdasm.c
MUSAINC=-DMUSASHI_CNF=\"../../include/m68kconf-emu68k.h\"

CFLAGS=-g -Iinclude -Wall -Wfatal-errors -std=c99 $(shell pkg-config --cflags expat gmodule-2.0)
CFLAGS+=-g -I ../raylib/src $(MUSAINC)

LDFLAGS= $(shell pkg-config --libs gmodule-2.0)
LDFLAGS+=-g -L ../raylib/src -lm -lraylib -lX11 -ldl -pthread
LDPLUG=-g $(shell pkg-config --libs gmodule-2.0) -ldl

INC=-Iinclude -I$(MUSA) -I ../raylib/src

SRC=$(wildcard src/*.c)
OBJ=$(SRC:src/%.c=obj/%.o)

ASM=$(wildcard target/asm/*.asm)
ASMs=$(ASM:target/asm/%.asm=target/asm/%.s)
ASMb=$(ASM:target/asm/%.asm=target/asm/%.bin)

PLUGSRC:= $(wildcard plug-src/*.c)
PLUGS:= $(PLUGSRC:.c=.so)
PLUGDEST:= $(subst plug-src,plugins,$(PLUGS))


CC=gcc
A68K=support/a68k/A68k
SREC=support/srec/srec2bin

.PHONY: all
all: $(MUSA)/m68kmake emu68k plugins asm

.PHONY: asm
asm: $(ASMb)


emu68k: $(MUSA)/m68kmake $(OBJ) $(MUSAOBJ)
	$(CC) $(MUSAOBJ) $(OBJ) -o emu68k $(LDFLAGS)

$(MUSA)/m68kmake:
	gcc $(MUSAINC) -o $(MUSA)/m68kmake $(MUSA)/m68kmake.c
	cd $(MUSA) && ./m68kmake
	
$(OBJ): obj/%.o : src/%.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
	
$(ASMs): target/asm/%.s : target/asm/%.asm 
	$(A68K) -n -l -s $<

$(ASMb): target/asm/%.bin : target/asm/%.s 
	$(SREC) -i $< -o $@

plugins: $(PLUGDEST)

plugins/%.so: plug-src/%.c
	gcc -o $@ -fPIC -shared $(CFLAGS) $< $(LDPLUG)
	

clean:
	rm -rf emu68k
	rm -rf obj/*
	rm -rf asm-src/*.bin
	rm -rf asm-src/*.s
	rm -rf plugins/*.so
	rm -rf $(MUSA)/*.o
	rm -rf $(MUSA)/softfloat/*.o	

cleanMus: clean
	rm -rf $(MUSA)/m68kops.*
	rm -rf $(MUSA)/m68kmake
	


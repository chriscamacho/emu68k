CFLAGS=-g -Iinclude -Wall -Wfatal-errors -std=c99 $(shell pkg-config --cflags expat gmodule-2.0)
CFLAGS+=-g -I ../raylib/src
#LDFLAGS=support/Musashi/libmusashi.a $(shell pkg-config --libs gmodule-2.0)
LDFLAGS= $(shell pkg-config --libs gmodule-2.0)
LDFLAGS+=-g -L ../raylib/src -lm -lraylib -lX11 -ldl -pthread
LDPLUG=-g $(shell pkg-config --libs gmodule-2.0) -ldl

INC=-Iinclude -Isupport/Musashi -I ../raylib/src

SRC=$(wildcard src/*.c)
OBJ=$(SRC:src/%.c=obj/%.o)

MUSA=support/Musashi/
MUSAOBJ=$(MUSA)/m68kcpu.o $(MUSA)/m68kops.o $(MUSA)/softfloat/softfloat.o $(MUSA)/m68kdasm.o

ASM=$(wildcard asm-src/*.asm)
ASMs=$(ASM:asm-src/%.asm=asm-src/%.s)
ASMb=$(ASM:asm-src/%.asm=asm-src/%.bin)
PLUGSRC:= $(wildcard plug-src/*.c)
PLUGS:= $(PLUGSRC:.c=.so)
PLUGDEST:= $(subst plug-src,plugins,$(PLUGS))


CC=gcc
A68K=support/a68k/A68k
SREC=support/srec/srec2bin

.PHONY: all
all: asm emu68k plugins

.PHONY: asm
asm: $(ASMb)


emu68k: $(OBJ)
	$(CC) $(MUSAOBJ) $(OBJ) -o emu68k $(LDFLAGS)

$(OBJ): obj/%.o : src/%.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
	
$(ASMs): asm-src/%.s : asm-src/%.asm 
	$(A68K) -l -s $<

$(ASMb): asm-src/%.bin : asm-src/%.s 
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

	


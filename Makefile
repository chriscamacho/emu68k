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

PLUGSRC:= $(wildcard src/plugins/*.c)
PLUGS:= $(PLUGSRC:.c=.so)
PLUGS:= $(subst src/plugins,plugins,$(PLUGS))

CC=gcc
A68K=support/a68k/A68k
SREC=support/srec/srec2bin

.PHONY: all
all: $(MUSA)/m68kmake emu68k $(OBJ) $(PLUGS) asm

	
.PHONY: asm
asm: $(ASMb)

emu68k: $(MUSA)/m68kmake $(OBJ) $(MUSAOBJ)
	@echo 
	@echo "linking emu68k"
	@echo 
	$(CC) $(MUSAOBJ) $(OBJ) -o emu68k $(LDFLAGS)
	@echo ------------------------------------------------------------------------

$(MUSA)/m68kmake:
	@echo 
	@echo "compile & run m68kmake"
	@echo 
	gcc $(MUSAINC) -o $(MUSA)/m68kmake $(MUSA)/m68kmake.c
	cd $(MUSA) && ./m68kmake
	@echo ------------------------------------------------------------------------

$(MUSAOBJ): $(MUSA)/%.o : $(MUSA)/%.c
	@echo 
	@echo "compile $(<) to $(@)" 
	@echo 
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
	@echo ------------------------------------------------------------------------
	
$(OBJ): obj/%.o : src/%.c
	@echo 
	@echo "compile $(<) to $(@)" 
	@echo 
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
	@echo ------------------------------------------------------------------------
	
$(ASMs): target/asm/%.s : target/asm/%.asm 
	@echo 
	@echo "assemble $(<) to $(@)" 
	@echo 
	$(A68K) -n -l -s $<
	@echo ------------------------------------------------------------------------

$(ASMb): target/asm/%.bin : target/asm/%.s 
	@echo 
	@echo "convert $(<) to $(@)" 
	@echo 
	$(SREC) -i $< -o $@
	@echo ------------------------------------------------------------------------

$(PLUGS): plugins/%.so : src/plugins/%.c
	@echo 
	@echo "compile $(<) to $(@)" 
	@echo 
	gcc -o $@ -fPIC -shared $(CFLAGS) $< $(LDPLUG)
	@echo ------------------------------------------------------------------------

plugins: $(PLUGS)



clean:
	rm -rf emu68k
	rm -rf obj/*
	rm -rf target/asm/*.bin
	rm -rf target/asm/*.s
	rm -rf target/asm/*.lst
	rm -rf plugins/*.so


cleanMus: clean
	rm -rf $(MUSA)/*.o
	rm -rf $(MUSA)/softfloat/*.o	
	rm -rf $(MUSA)/m68kops.*
	rm -rf $(MUSA)/m68kmake
	


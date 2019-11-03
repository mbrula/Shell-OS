include ../Makefile.inc

MODULE=0000-phyloView.bin
SOURCES=$(wildcard src/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c

LIB_SOURCES=$(wildcard ../Library/src/*.c)
LIB_ASM=$(wildcard ../Library/asm/*.o)

LOADEROBJECT=$(LOADERSRC:.c=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -I./include -I../Library/include -T phyloView.ld _loader.c $(SOURCES) $(OBJECTS_ASM) $(LIB_ASM) $(LIB_SOURCES) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -I./include -I../Library/include -T phyloView.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) $(OBJECTS_ASM) $(LIB_ASM) $(LIB_SOURCES) -o ../0000-phyloView.elf


%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../Library/include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o src/*.o *.elf *.bin

.PHONY: all clean print

#$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
#	$(LD) $(LDFLAGS) -T phylo.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

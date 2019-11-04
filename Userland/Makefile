include Makefile.inc

all: library processA processB shell idle sleep loop cat wc filter phylo phyloProcess phyloView

library:
	cd Library; make

processA:
	cd ProcessA; make

processB:
	cd ProcessB; make

shell:
	cd Shell; make

idle:
	cd Idle; make

sleep:
	cd Sleep; make	

loop:
	cd Loop; make

cat:
	cd Cat; make	

wc:
	cd Wc; make	

filter:
	cd Filter; make

phylo:
	cd Phylo; make

phyloProcess:
	cd PhyloProcess; make

phyloView:
	cd PhyloView; make

clean:
	cd Library; make clean
	cd ProcessA; make clean
	cd ProcessB; make clean
	cd Shell; make clean
	cd Idle; make clean
	cd Sleep; make clean
	cd Loop; make clean
	cd Cat; make clean
	cd Wc; make clean
	cd Filter; make clean
	cd Phylo; make clean
	cd PhyloProcess; make clean
	cd PhyloView; make clean
	rm -rf *.bin


.PHONY: library processA processB shell idle sleep loop cat wc filter phylo phyloProcess phyloView all clean

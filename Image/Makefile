BOOTLOADER_PATH=../Bootloader
BMFS=$(BOOTLOADER_PATH)/BMFS/bmfs.bin
MBR=$(BOOTLOADER_PATH)/Pure64/bmfs_mbr.sys
MP=../Toolchain/ModulePacker/mp.bin
PURE64=$(BOOTLOADER_PATH)/Pure64/pure64.sys
OSIMAGENAME=x64BareBonesImage
VMDK=$(OSIMAGENAME).vmdk
QCOW2=$(OSIMAGENAME).qcow2
IMG=$(OSIMAGENAME).img
KERNEL=../Kernel/kernel.bin
USERLAND=../Userland/0000-shell.bin ../Userland/0000-processA.bin ../Userland/0000-processB.bin ../Userland/0000-idle.bin ../Userland/0000-sleep.bin ../Userland/0000-loop.bin ../Userland/0000-cat.bin ../Userland/0000-wc.bin ../Userland/0000-filter.bin ../Userland/0000-phylo.bin ../Userland/0000-phyloProcess.bin ../Userland/0000-phyloView.bin

PACKEDKERNEL=packedKernel.bin
IMGSIZE=6291456

all: $(IMG) $(VMDK) $(QCOW2)

$(KERNEL):
	cd ../Kernel; make

$(PACKEDKERNEL): $(KERNEL) $(USERLAND)
	$(MP) $(KERNEL) $(USERLAND) -o $(PACKEDKERNEL)

$(IMG): $(BMFS) $(MBR) $(PURE64) $(PACKEDKERNEL)
	$(BMFS) $(IMG) initialize $(IMGSIZE) $(MBR) $(PURE64) $(PACKEDKERNEL) 

$(VMDK): $(IMG)
	qemu-img convert -f raw -O vmdk $(IMG) $(VMDK) 

$(QCOW2): $(IMG)
	qemu-img convert -f raw -O qcow2 $(IMG) $(QCOW2)

clean:
	rm -rf $(IMG) $(VMDK) $(QCOW2) *.bin

.PHONY: all clean

# Shell Operating System

### A shel oparating system based on x64BareBones

#### Environment setup:
1. Install the following packages before building the Toolchain and Kernel:
  `nasm qemu gcc make`

2. Build the Toolchain (on x64BareBones project directory):
  `cd Toolchain`
  `make all`

3. Build the Kernel (on x64BareBones project directory):
  `make all`

4. Run the kernel
  `./run.sh`

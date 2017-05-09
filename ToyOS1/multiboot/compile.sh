gcc -I. -ffreestanding -nostdlib -c boot.S kernel.c

# -Ttext 0x100000 : obj is ready to be loaded in memory location 0x100000 (first 1Mb)
# --strip-all : symbol inofs are removed from obj file
ld -Ttext 0x100000 -o mkernel.bin boot.o kernel.o 

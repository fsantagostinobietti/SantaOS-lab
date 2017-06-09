TOOLCHAIN='../cross-compiler'
ARCH='i386-linux'

# clean up
rm -f *.o

echo '* compile ASM boot code.'
$TOOLCHAIN/bin/$ARCH-as boot.s -o boot.o
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm -a boot.o

echo ''
echo '* compile C code.'
$TOOLCHAIN/bin/$ARCH-gcc -c *.c -std=gnu99 -ffreestanding -O2 -Wall -Wextra
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm -a kernel.o
#echo '* assembly code: kernel.asm'
#$TOOLCHAIN/bin/$ARCH-gcc -S -c kernel.c -o kernel.asm -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo ''
echo '* link all together.'
# boot.o asm.o std.o tty.o serial.o tcpip.o kernel.o
$TOOLCHAIN/bin/$ARCH-gcc -T linker.ld -o mytoyos.bin -ffreestanding -O2 -nostdlib *.o -lgcc
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm -S -n mytoyos.bin

echo ''
if grub-file --is-x86-multiboot mytoyos.bin; then
  echo '* multiboot confirmed'
else
  echo '! mytoyos.bin file is not multiboot'
  exit 1
fi


echo ''
echo '* create bootable iso image.'
mkdir -p isodir/boot/grub
cp mytoyos.bin isodir/boot/mytoyos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o mytoyos.iso isodir
rm -rf isodir


echo ''
echo "* now you can test mytoyos with '$ qemu-system-i386 -cdrom mytoyos.iso'."

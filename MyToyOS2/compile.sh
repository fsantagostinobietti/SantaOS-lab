TOOLCHAIN='../cross-compiler'
ARCH='i386-linux'

echo '* compile ASM boot code.'
$TOOLCHAIN/bin/$ARCH-as boot.s -o boot.o
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm -a boot.o

echo ''
echo '* compile C kernel code.'
$TOOLCHAIN/bin/$ARCH-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm -a kernel.o
echo '* assembly code: kernel.asm'
$TOOLCHAIN/bin/$ARCH-gcc -S -c kernel.c -o kernel.asm -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo ''
echo '* link all together.'
$TOOLCHAIN/bin/$ARCH-gcc -T linker.ld -o mytoyos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
#echo '* list symbols:'
#$TOOLCHAIN/bin/$ARCH-nm mytoyos.bin

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

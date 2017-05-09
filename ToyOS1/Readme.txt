# Create empty file
dd if=/dev/zero obs=512 count=2880 of=fat_floppy.img
# Format file as floppy disk image, using FAT filesystem
mformat -f 1440 -i fat_floppy.img ::

# ... or
mkfs.vfat -C fat_floppy.img 1440
# Dir
mdir -i fat_floppy.img /

# Create boot/grub subdirs
mmd -i fat_floppy.img boot
mmd -i fat_floppy.img boot/grub

# Copy grub files
mcopy  /usr/lib/grub/i386-pc/stage* -i fat_floppy.img ::/boot/grub
mdir -i fat_floppy.img boot/grub
# Copy menu.lst file
mcopy  menu.lst -i fat_floppy.img ::/boot/grub

# Edit menu.lst file as you need ..
*menu.lst*
--------------------------------------
# Boot automatically after 30 secs.
timeout 30
# By default, boot the first entry.
default 0

# MyToy OS (multiboot)
title MyToyOS (multiboot)
root (fd0)
kernel /kernel32.bin

# MyToyOS (bootchain)
title MyToyOS (bootchain)
root (fd0)
chainloader /loader.bin
---------------------------------------


# Install grub on floppy image
$> sudo grub
grub> device (fd0) /path/to/fat_floppy.img
grub> root (fd0)
grub> setup (fd0)


How to mount floppy image
-------------------------
sudo mkdir /mnt/myFloppy
sudo mount -t vfat -o loop fat_floppy.img /mnt/myFloppy
...
sudo umount /mnt/myFloppy





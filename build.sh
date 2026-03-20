set -e

nasm -f elf32 boot.s -o boot.o

gcc -m32 -c kernel.c -o kernel.o -ffreestanding -O2 -Wall -fno-stack-protector

ld -m elf_i386 -T linker.ld boot.o kernel.o -o myos.bin

mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/grub/myos.bin
cat << EOF > isodir/boot/grub/grub.cfg
menuentry "Obsidian OS" {
    multiboot /boot/grub/myos.bin
}
EOF
grub-mkrescue -o myos.iso isodir

qemu-system-i386 -cdrom myos.iso -m 64M

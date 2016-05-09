## By Whirligig231

all: whirlios.img

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_lib.o: kernel_lib.asm
	as86 -o kernel_lib.o kernel_lib.asm
	
kernel.e: kernel.o kernel_lib.o
	ld86 -o kernel.e -d kernel.o kernel_lib.o
	
bootload.e: bootload.asm
	nasm -o bootload.e bootload.asm
	
whirlios.img: bootload.e kernel.e test1 test2
	dd if=/dev/zero of=whirlios.img bs=512 count=2880
	dd if=bootload.e of=whirlios.img bs=512 count=1 conv=notrunc
	dd if=kernel.e of=whirlios.img bs=512 conv=notrunc seek=2
	dd if=test1 of=whirlios.img bs=512 conv=notrunc seek=16
	dd if=test2 of=whirlios.img bs=512 conv=notrunc seek=17
	
test: whirlios.img
	bochs -f ubuntu.bxrc
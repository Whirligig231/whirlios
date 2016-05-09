## By Whirligig231

all: whirlios.img

diskutility.o: diskutility.c
	gcc -std=c99 -c -g -o diskutility.o diskutility.c
	
diskutility: diskutility.o
	gcc -std=c99 -g -o diskutility diskutility.o

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

kernel_lib.o: kernel_lib.asm
	as86 -o kernel_lib.o kernel_lib.asm
	
kernel.e: kernel.o kernel_lib.o
	ld86 -o kernel.e -d kernel.o kernel_lib.o
	
bootload.e: bootload.asm
	nasm -o bootload.e bootload.asm
	
lib.o: lib.asm
	as86 -o lib.o lib.asm
	
wsh.o: wsh.c
	bcc -ansi -c -o wsh.o wsh.c
	
wsh.e: wsh.o lib.o
	ld86 -o wsh.e -d wsh.o lib.o
	
whirlios.img: diskutility bootload.e kernel.e test.txt wsh.e
	dd if=/dev/zero of=whirlios.img bs=512 count=2880
	dd if=bootload.e of=whirlios.img bs=512 count=1 conv=notrunc
	dd if=kernel.e of=whirlios.img bs=512 conv=notrunc seek=2
	./diskutility
	
test: whirlios.img
	bochs -f ubuntu.bxrc
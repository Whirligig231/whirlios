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

lib/syscall.o: lib/syscall.asm
	as86 -o lib/syscall.o lib/syscall.asm

lib/ilib.o: lib/ilib.c lib/flib.h
	bcc -ansi -c -o lib/ilib.o lib/ilib.c

lib/mlib.o: lib/mlib.c lib/mlib.h
	bcc -ansi -c -o lib/mlib.o lib/mlib.c

lib/slib.o: lib/slib.c lib/mlib.h
	bcc -ansi -c -o lib/slib.o lib/slib.c

lib/flib.o: lib/flib.c lib/slib.h lib/flib.h
	bcc -ansi -c -o lib/flib.o lib/flib.c

wsh.o: wsh.c lib/ilib.h lib/slib.h lib/flib.h
	bcc -ansi -c -o wsh.o wsh.c

wsh.e: wsh.o lib/syscall.o lib/ilib.o lib/mlib.o lib/slib.o lib/flib.o
	ld86 -o wsh.e -d wsh.o lib/syscall.o lib/ilib.o lib/mlib.o lib/slib.o lib/flib.o

programs/hello.o: programs/hello.c lib/ilib.h
	bcc -ansi -c -o programs/hello.o programs/hello.c

programs/hello.e: programs/hello.o lib/syscall.o lib/ilib.o
	ld86 -o programs/hello.e -d programs/hello.o lib/syscall.o lib/ilib.o

programs/hello2.o: programs/hello2.c lib/ilib.h
	bcc -ansi -c -o programs/hello2.o programs/hello2.c

programs/hello2.e: programs/hello2.o lib/syscall.o lib/ilib.o
	ld86 -o programs/hello2.e -d programs/hello2.o lib/syscall.o lib/ilib.o

programs/calc.o: programs/calc.c lib/ilib.h lib/mlib.h lib/slib.h
	bcc -ansi -c -o programs/calc.o programs/calc.c

programs/calc.e: programs/calc.o lib/syscall.o lib/ilib.o lib/mlib.o lib/slib.o
	ld86 -o programs/calc.e -d programs/calc.o lib/syscall.o lib/ilib.o lib/mlib.o lib/slib.o

whirlios.img: diskutility bootload.e kernel.e test.txt wsh.e programs/hello.e programs/hello2.e programs/calc.e
	dd if=/dev/zero of=whirlios.img bs=512 count=2880
	dd if=bootload.e of=whirlios.img bs=512 count=1 conv=notrunc
	dd if=kernel.e of=whirlios.img bs=512 conv=notrunc seek=2
	./diskutility

test_ubuntu: whirlios.img
	bochs -f ubuntu.bxrc

test_win32: whirlios.img
	bochs -f win32.bxrc

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.e" -type f -delete
	find . -name "*.img" -type f -delete

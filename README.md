# WhirliOS
Whirligig231's 16-Bit Operating System

**N.B.** Most of the info below currently relates to features planned for v0.1. Expect next to none of it to actually be implemented yet.

## Building and Installing WhirliOS

WhirliOS can be built using the included Makefile, which requires the following tools:

* bcc (Bruce's C Compiler): for compiling the operating system's C code
* as86: for assembling some of the 16-bit ASM code
* nasm: for assembling some of the other ASM code
* ld86: for linking the machine code files
* dd: for copying files onto the disk drive
* gcc: for compiling utilities

Once whirlios.img has been saved, you can copy it onto a 3.5", 1.44MB floppy disk using your favorite disk-imaging utility. Then simply reboot and tell your BIOS to boot from the floppy.

Alternatively, if you don't want to have to restart your entire machine every time you want to test the OS, you can use Bochs to emulate it instead. Two configuration (.bxrc) files are provided: one for Bochs 2.4.6 under Ubuntu and one for Bochs 2.6.8 under Windows 7. If you want to use a different version of Bochs or emulate under a different OS, you will have to create the .bxrc yourself.

## Using WhirliOS

Once WhirliOS has booted, you will see a mostly black screen with a "/>" prompt. This is the shell. Type in a command and hit ENTER to use the shell. The following commands are supported:

* cat: print file contents
* cd: change directory
* chf: change file flags
* cp: copy a file or directory
* echo: print input
* help: get a list of commands that can be used
* ls: list contents of the current directory
* mk: make a file by typing it in
* mkd: make a directory
* pwd: print the working directory
* reboot: reboot the system
* rm: remove a file or directory

You can also type the name of a program to run it. Programs are stored in the /bin directory; this is the equivalent of $PATH on Unix-like systems and cannot be changed. Alternatively, you can type a file path to run a program somewhere else.

File paths support the typical Unix-like features with the exception of ~, as there is no home directory.

## The File System

WhirliOS uses its own file system, which is not compatible with FAT, NTFS, EXT, or any other file system. The system assumes 512B sectors. Sector 0 is reserved for the bootloader, and sectors 2-15 are reserved for the kernel.

Sector 1 is called the sector usage table (SUT); it keeps track of which sectors on the disk are in use. Each sector is represented by a bit, which is set to either 1 for used or 0 for unused. As the first 16 sectors are reserved, the first two bytes of the SUT will always be FF FF.

Note that as the SUT has 512 x 8 = 4096 bits, only 4096 sectors on the disk are actually usable. However, a 1.44MB floppy disk only has 2880 sectors, so even this limit cannot be reached on a typical floppy.

Each file or directory begins with the eight-byte _file descriptor._ The eight bytes correspond to the eight allowed characters of the file's name. Note that as unextended ASCII is a seven-bit encoding, only the low seven bits of each byte are used for naming. The top bits form a bitfield of eight flags, giving information about the file.

The first two flags indicate the indexing mode of the file. If both flags are off (0b00), the file uses no indexing. This means that the entire file is stored in a single sector. Thus, the maximum file size for this scheme is 504 B. If the first flag is off and the second is on (0b01), the file uses simple indexing. This means that the remainder of this sector contains, rather than the file's contents, a list of other sectors on the disk in which the file is stored, each indexed by twelve bits (so that two sectors fit into three bytes). Thus, the maximum file size for this scheme is 172 KB. If the first flag is on and the second is off (0b10), the file uses complex indexing, in which the remainder of the first sector indexes sectors that may themselves be index sectors; this mode is currently not supported. The case where both flags are on (0b11) is currently reserved for further expansion.

The next two flags, flags 2 and 3, indicate the type of the file. 0b00 indicates that the file is a text file, while 0b01 indicates that the file is a non-executable binary. 0b10 indicates that the file is an executable. 0b11 indicates a directory; in WhirliOS, directories are also files.

Flag 4 indicates that the file is locked for writing; this is not secure, as a simple use of the chf command can change it, but it helps to prevent the user from accidentally deleting or corrupting system files. Flags 5, 6, and 7 are reserved for further expansion.

As an example, consider a file whose first eight bytes are 77 F3 E8 00 80 00 00 00. We may recover the name of the file by stripping the MSB from each byte, producing 77 73 68 00 00 00 00 00. This tells us that "wsh" is the name of the file. The MSB's are the flags, 01101000. The first two, 01, indicate that this file uses simple indexing. The next two, 10, indicate that the file is an executable. The next flag, 1, indicates that the file is read-only. (In fact, this is the shell.)

Directories are simply files containing the indices of sectors, two bytes to a sector, at which other files begin. Note that marking a directory as read-only does not affect the files inside it.

File lengths are restricted to lengths that fill sectors. An unindexed file is always 504 B in size, while an indexed file's size is always a multiple of 512 B. Read/write functions work one sector at a time; for unindexed files, the last eight bytes will always be 00 00 00 00 00 00 00 00.

## Writing Code for WhirliOS

WhirliOS does not support any of the standard C libraries (stdio, stdlib, etc.), instead providing its own. These are named by letters of the alphabet with "lib" after them, and each has a header file. The exception is lib.asm, which provides very low-level support and is compiled as a raw binary file; use of lib.asm directly is discouraged, as you can use the other libraries instead to access the corresponding functionality at a higher level. These include:

* alib.h: audio support, for beeping the internal speaker
* dlib.h: dynamic memory allocation (TODO)
* flib.h: file I/O support
* glib.h: graphics support (mode 13h)
* ilib.h: console I/O support
* mlib.h: mathematical functions
* slib.h: string parsing
* tlib.h: timekeeping functions
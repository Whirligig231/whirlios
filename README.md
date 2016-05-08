# whirlios
Whirligig231's 16-Bit Operating System

**N.B.** Most of the info below currently relates to features planned for v0.1. Expect next to none of it to actually be implemented yet.

## Building and Installing WhirliOS

WhirliOS can be built using the included Makefile, which requires the following tools:

* bcc (Bruce's C Compiler): for compiling the operating system's C code
* as86: for assembling the 16-bit ASM code
* ld86: for linking the machine code files
* dd: for copying files onto the disk drive
* gcc: for compiling utilities

Once whirlios.img has been saved, you can copy it onto a 3.5", 1.44MB floppy disk using your favorite disk-imaging utility. Then simply reboot and tell your BIOS to boot from the floppy.

Alternatively, if you don't want to have to restart your entire machine every time you want to test the OS, you can use Bochs to emulate it instead. Two configuration (.bxrc) files are provided: one for Bochs 2.4.6 under Ubuntu and one for Bochs 2.6.8 under Windows 7. If you want to use a different version of Bochs or emulate under a different OS, you will have to create the .bxrc yourself.
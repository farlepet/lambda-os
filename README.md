Lambda OS
=========

This repository combines the Lambda kernel, userspace library, and a small set
of userspace utilities to act as a "reference implementation" of sorts for the
Lambda kernel.

Building
--------

To be able to build Lambda OS, you need the following tools:
 - `gcc` or `clang` (Use cross-compiler if necessary)
 - `make`
 - `cpio`
 - `eu-readelf` (elfutils)
 - For X86 target
   - For floppy image:
     - `mkdosfs`
     - `mcopy`
     - `syslinux`
       - NOTE: This expects syslinux modules to be located in `/usr/lib/syslinux/bios/`
   - For ISO image:
     - `xorriso`
     - `grub-mkrescue`
     - GRUB `stage2_eltorito` should be placed in `build/CD/boot/grub`
       - This can be found online, or from an installation of GRUB legacy

To build Lambda OS:
 - If fisrt time
   - Clone the `lambda-os` repository.
   - Update submodules: `git submodule update --init`
 - If not first time
   - `git pull`
   - `git submodule update`
 - Make: `make`
   - To speed up compilation, add `-j<threads>` argument to the `make` command
   - Defaults to x86 ISO
     - For clang: `make CC=clang AS=clang`
     - 1.44MiB floppy target: `make floppy`
       - NOTE: This currently requires a manual change in the kernels config.h:
         `FEATURE_MULTIBOOT` must be set to 1, rather than 2.
     - ISO target: `make cdrom`
   - For ARMv7: `make ARCH=armv7 CROSS_COMPILE=<gcc prefix>`
     - **NOTE**: ARMv7 target currently still creates x86 ISO, no image format
       has yet been selected for this architecture.

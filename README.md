Lambda OS
=========

This repository combines the Lambda kernel, userspace library, and a small set
of userspace utilities to act as a "reference implementation" of sorts for the
Lambda kernel.

Building
--------

To be able to build Lambda OS, you need the following tools:
 - `gcc` or `clang` (Use cross-compiler if necessary)
 - `make` (or `gmake` on BSD systems)
 - `cpio`
 - `eu-readelf` (elfutils)
 - For X86 target
   - For floppy image:
     - `mkdosfs`
     - `mmd` + `mcopy` from `mtools`
   - For ISO image:
     - `xorriso`
     - `grub-mkrescue`
     - GRUB `stage2_eltorito` should be placed in `build/CD/boot/grub`
       - This can be found online, or from an installation of GRUB legacy

Alternatively, the included `build-wrapper.sh` script can be used instead
 - Target platform (the GCC/binutils target) is set via the `LAMBDA_TARGET` environment variable
   - Defaults to `i686-elf`
   - Available options are those with tag `latest-<taget>` at https://git.pfarley.dev/lambda-os/-/packages/container/builder/
 - For example: `build-wrapper.sh make floppy`
   - `build-wrapper.sh` will automatically set the `CROSS_COMPILE` variable for the chosen target
   - Clang not currently supported via this route

To build Lambda OS:
 - If first time
   - Clone the `lambda-os` repository.
   - Update submodules: `git submodule update --recursive --init`
 - If not first time
   - `git pull`
   - `git submodule update`
 - Make: `make`
   - To speed up compilation, add `-j<threads>` argument to the `make` command
   - Defaults to floppy
     - For clang:
       - Requires special kernel config
         - `make menuconfig`
           - Assumes menuconfig tool is installed
       - Not fully supported at this time
     - 1.44MiB floppy target: `make floppy`
     - ISO target: `make cdrom`
   - For ARMv7: `make ARCH=armv7 CROSS_COMPILE=<gcc prefix>`
     - **NOTE**: ARMv7 target currently still creates x86 ISO, no image format
       has yet been selected for this architecture.

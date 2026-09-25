@echo off

qemu-system-x86_64 ^
    -machine q35 ^
    -m 256M ^
    -drive format=raw,file=build\zenith.img ^
    -bios tools\OVMF.fd ^
    -serial stdio

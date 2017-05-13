CFLAGS = -Wall -Wextra -e efi_main -nostdlib -Wl,--subsystem,10	\
-Iinclude

all: fs/EFI/BOOT/BOOTX64.EFI

fs/EFI/BOOT/BOOTX64.EFI: common.c efi.c console.c shell.c main.c
	mkdir -p fs/EFI/BOOT
	x86_64-w64-mingw32-gcc $(CFLAGS) -o $@ $+

run: fs/EFI/BOOT/BOOTX64.EFI
	qemu-system-x86_64 -bios OVMF.fd -hda fat:fs

clean:
	rm -rf *~ fs

.PHONY: clean

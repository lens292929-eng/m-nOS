import os
import fs
from pyfatfs.PyFat import PyFat


IMG_PATH = "build/zenith.img"
EFI_PATH = "build/BOOTX64.EFI"
KERNEL_PATH = "build/KERNEL.BIN"


def main():
    print("[1/3] Formatting FAT32...")

    os.makedirs("build", exist_ok=True)

    # Pre-allocate 64 MiB raw image file
    with open(IMG_PATH, "wb") as f:
        f.truncate(64 * 1024 * 1024)

    # Format the image using PyFat directly
    fat = PyFat()
    fat.mkfs(IMG_PATH, fat_type=32)
    fat.close()

    print("[2/3] Opening filesystem via PyFilesystem2...")

    # Open using PyFilesystem2's FAT opener via context manager
    with fs.open_fs(f"fat://{IMG_PATH}") as fat_fs:
        # Create directory hierarchy
        fat_fs.makedirs("/EFI/BOOT", recreate=True)

        print("[3/3] Copying EFI files...")

        with open(EFI_PATH, "rb") as f:
            fat_fs.writebytes("/EFI/BOOT/BOOTX64.EFI", f.read())

        with open(KERNEL_PATH, "rb") as f:
            fat_fs.writebytes("/EFI/BOOT/KERNEL.BIN", f.read())

    print()
    print("========================================")
    print("        EFI IMAGE CREATED")
    print("========================================")
    print()
    print("  " + IMG_PATH)


if __name__ == "__main__":
    main()
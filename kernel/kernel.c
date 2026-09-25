#include "framebuffer.h"
#include "colors.h"
#include "keyboard.h"
#include "idt.h" 
#include "io.h"

static void show_welcome(void)
{
    print_set_colors(COLOR_LIGHT_CYAN, COLOR_BLACK);
    print(
        "\n"
        "==========================================\n"
        " m!nOS\n"
        "==========================================\n"
        "\n");
    print(
        "Welcome to m!nOS! m!nOS is a 64-bit Operating System "
        "made by lens24!\n"
        "\n");
}

static void show_logs(void)
{
    print_tagged("OK", COLOR_LIGHT_GREEN, "Kernel Reached\n");
}

__attribute__((ms_abi))
void kmain(BootInfo *boot_info)
{
    framebuffer_init(boot_info);
    framebuffer_clear(COLOR_BLACK);

    show_welcome();
    show_logs();
    idt_init();               // zero + set idtp.base = &idt
    idt_install_handlers();   // now populate
    pic_remap();
    keyboard_init();
    idt_activate();               // <-- this was missing entirely!
    __asm__ volatile ("sti");              // <-- this was missing entirely!

    print_set_colors(COLOR_WHITE, COLOR_BLACK);
    print("m!n@liveimg ~ $ ");

    for (;;) {
    if (inb(0x64) & 0x01) {          // output buffer full
            u8 sc = inb(0x60);
            keyboard_handle_scancode(sc); // call directly, no IRQ
        }
    }
}
#include "isr.h"
#include "io.h"
#include "keyboard.h"
#include "framebuffer.h"

void isr_dispatch(registers_t *regs)
{
    if (regs->vector == 0x21) {
        printc('K');
        u8 scancode = inb(0x60);
        keyboard_handle_scancode(scancode);
        outb(0x20, 0x20);
        return;
    }
}
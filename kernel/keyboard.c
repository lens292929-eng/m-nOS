#include "keyboard.h"
#include "io.h"
#include "framebuffer.h"
#include "colors.h"
#include "shell.h"

// US QWERTY, PS/2 Set 1 make codes.
static const char keymap[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q',  'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,    '*',  0,   ' ',
};

static const char keymap_shift[128] = {
    0,    27,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,    '*',  0,   ' ',
};

static int shift = 0;
static int caps  = 0;

#define LINE_MAX 256
static char line[LINE_MAX];
static int  line_len = 0;

void keyboard_init(void)
{
    /*
     * Enable the first PS/2 port.
     */

    // Wait until controller input buffer is free.
    while (inb(0x64) & 0x02);

    outb(0x64, 0xAE);

    /*
     * Read controller configuration byte.
     */

    while (inb(0x64) & 0x02);
    outb(0x64, 0x20);

    // Wait for output buffer.
    while (!(inb(0x64) & 0x01));

    u8 cfg = inb(0x60);

    // Enable IRQ1.
    cfg |= 0x01;

    // Enable clock for first PS/2 port.
    cfg &= ~0x10;

    /*
     * Write configuration byte back.
     */

    while (inb(0x64) & 0x02);
    outb(0x64, 0x60);

    while (inb(0x64) & 0x02);
    outb(0x60, cfg);

    /*
     * Flush pending keyboard data.
     */

    while (inb(0x64) & 0x01)
        (void)inb(0x60);

    shift = 0;
    caps  = 0;
    line_len = 0;
}

void keyboard_handle_scancode(u8 sc)
{
    /*
     * Set 1 extended scancodes.
     *
     * 0xE0 means the next scancode is an extended key.
     * We don't support those yet, so just ignore them.
     */
    static int extended = 0;

    if (sc == 0xE0) {
        extended = 1;
        return;
    }

    if (extended) {
        extended = 0;
        return;
    }

    /*
     * Key release.
     * Set 1 uses bit 7 to indicate release.
     */
    if (sc & 0x80) {
        u8 make = sc & 0x7F;

        if (make == 0x2A || make == 0x36)
            shift = 0;

        return;
    }

    /*
     * Shift.
     */
    if (sc == 0x2A || sc == 0x36) {
        shift = 1;
        return;
    }

    /*
     * Caps Lock.
     */
    if (sc == 0x3A) {
        caps = !caps;
        return;
    }

    /*
     * Ignore invalid scancodes.
     */
    if (sc >= 128)
        return;

    /*
     * Get the character.
     */
    char c = shift ? keymap_shift[sc] : keymap[sc];

    if (!c)
        return;

    /*
     * Caps Lock affects letters only.
     *
     * XOR gives the desired behavior:
     *
     * Caps       Shift       Result
     * off        off         lowercase
     * off        on          uppercase
     * on         off         uppercase
     * on         on          lowercase
     */
    if (c >= 'a' && c <= 'z') {
        if (caps ^ shift)
            c -= 'a' - 'A';
    }
    else if (c >= 'A' && c <= 'Z') {
        if (!(caps ^ shift))
            c += 'a' - 'A';
    }

    /*
     * Line editing.
     */

    if (c == '\n') {
        printc('\n');
        line[line_len] = '\0';
        shell_submit(line);      /* shell reprints the prompt itself */
        line_len = 0;
        return;
    }

    if (c == '\b') {
        if (line_len > 0) {
            line_len--;
            printc('\b');        /* printc erases the cell */
        }
        return;
    }

    /*
     * Ignore other control characters (tab, esc, etc.) for now.
     */
    if (c < 0x20 || c > 0x7E)
        return;

    if (line_len < LINE_MAX - 1) {
        line[line_len++] = c;
        printc(c);
    }
}
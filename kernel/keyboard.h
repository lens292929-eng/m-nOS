#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

void keyboard_init(void);
void keyboard_handle_scancode(u8 sc);

#endif
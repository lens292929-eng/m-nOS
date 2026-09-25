#include "framebuffer.h"
#include "shell.h"

void shell_prompt(void) {
    print_set_colors(0x00FFFFFF, 0x00000000);
    print("m!n@liveimg ~ $ ");
}

static int streq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static void cmd_help(void) {
    print("commands:\n");
    print("  help     - this list\n");
    print("  clear    - clear screen\n");
    print("  echo X   - print X\n");
    print("  version  - kernel version\n");
    print("  pizza    - do not run\n");
}

static void cmd_echo(const char *rest) {
    while (*rest == ' ') rest++;
    print(rest);
    printc('\n');
}

static void cmd_version(void) {
    print("m!nOS 0.0.1 kernel version 0.0.2\n");
}

void shell_pizza(void) {
    print("            88                                 \n");
    print("            \"\"                                 \n");
    print("                                                \n");
    print("8b,dPPYba,  88 888888888 888888888 ,adPPYYba,  \n");
    print("88P'    \"8a 88      a8P\"      a8P\" \"\"     `Y8  \n");
    print("88       d8 88   ,d8P'     ,d8P'   ,adPPPPP88  \n");
    print("88b,   ,a8\" 88 ,d8\"      ,d8\"      88,    ,88  \n");
    print("88`YbbdP\"'  88 888888888 888888888 `\"8bbdP\"Y8  \n");
    print("88                                              \n");
    print("88         \n");
}

void shell_submit(const char *line) {
    if (line[0] == '\0') {
        shell_prompt();
        return;
    }

    if      (streq(line, "help"))    cmd_help();
    else if (streq(line, "clear"))   framebuffer_clear(0x00000000);
    else if (streq(line, "version")) cmd_version();
    else if (streq(line, "pizza")) shell_pizza();
    else if (line[0]=='e' && line[1]=='c' && line[2]=='h' &&
             line[3]=='o' && line[4]==' ') cmd_echo(line + 5);
    else {
        print("unknown command: ");
        print(line);
        printc('\n');
    }

    shell_prompt();
}


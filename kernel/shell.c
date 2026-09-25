#include "framebuffer.h"
#include "shell.h"
#include "types.h"

void shell_prompt(void) {
    print_set_colors(0x00FFFFFF, 0x00000000);
    print("m!n@liveimg ~ $ ");
}

static int streq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static int strlen_simple(const char *s);

/* ---------- commands ---------- */

static void cmd_help(int argc, char **argv);
static void cmd_clear(int argc, char **argv);
static void cmd_echo(int argc, char **argv);
static void cmd_version(int argc, char **argv);
static void cmd_pizza(int argc, char **argv);
static void cmd_ls(int argc, char **argv);

/* ---------- command table ---------- */

struct command {
    const char *name;
    void (*fn)(int argc, char **argv);
    const char *help;
};

static const struct command commands[] = {
    { "help",    cmd_help,    "this list" },
    { "clear",   cmd_clear,   "clear screen" },
    { "echo",    cmd_echo,    "echo arguments" },
    { "version", cmd_version, "kernel version" },
    { "pizza",   cmd_pizza,   "do not run" },
    { "ls",      cmd_ls,      "list files" },
    { NULL, NULL, NULL },
};

/* ---------- implementations ---------- */

static void cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    print("commands:\n");
    for (int i = 0; commands[i].name; i++) {
        print("  ");
        print(commands[i].name);
        for (int n = 10 - (int)strlen_simple(commands[i].name); n > 0; n--)
            printc(' ');
        print("- ");
        print(commands[i].help);
        printc('\n');
    }
}

static void cmd_clear(int argc, char **argv) {
    (void)argc; (void)argv;
    framebuffer_clear(0x00000000);
}

static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        print(argv[i]);
        if (i + 1 < argc) printc(' ');
    }
    printc('\n');
}

static void cmd_version(int argc, char **argv) {
    (void)argc; (void)argv;
    print("m!nOS 0.0.1 kernel version 0.0.2\n");
}

static void cmd_pizza(int argc, char **argv) {
    (void)argc; (void)argv;
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

static void cmd_ls(int argc, char **argv) {
    (void)argc; (void)argv;
    print("system/\n");
    print("colors.h  font.c  font.h  framebuffer.c  framebuffer.h  "
          "idt.c  idt.h  idt_load.asm  io.h  isr.c  isr.h  "
          "isr_stubs.asm  kernel.c  kernel.ld  keyboard.c  keyboard.h  "
          "seabios_font.h  shell.c  shell.h  types.h\n");
}

/* ---------- string helpers ---------- */

/* local for now; move to string.c later */
static int strlen_simple(const char *s) {
    int n = 0;
    while (s[n]) n++;
    return n;
}

/* ---------- tokenizer ---------- */

#define ARGV_MAX 16

static int tokenize(char *line, char **argv) {
    int argc = 0;
    char *p = line;

    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (argc >= ARGV_MAX) break;

        argv[argc++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) *p++ = '\0';
    }
    return argc;
}

/* ---------- entry point ---------- */

void shell_submit(char *line) {
    char *argv[ARGV_MAX];
    int argc = tokenize(line, argv);

    if (argc == 0) {
        shell_prompt();
        return;
    }

    for (int i = 0; commands[i].name; i++) {
        if (streq(argv[0], commands[i].name)) {
            commands[i].fn(argc, argv);
            shell_prompt();
            return;
        }
    }

    print("unknown command: ");
    print(argv[0]);
    printc('\n');
    shell_prompt();
}
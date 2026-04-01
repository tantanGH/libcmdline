#include <x68k/dos.h>
#include <string.h>
#include <ctype.h>

#define MAX_CMDLINE (256)
#define MAX_ARGS (32)

/* Macro for Shift-JIS 1st byte check (XC-compatible) */
#define iskanji(c) (((unsigned char)(c) >= 0x81 && (unsigned char)(c) <= 0x9F) || \
                    ((unsigned char)(c) >= 0xE0 && (unsigned char)(c) <= 0xEF))

static char  s_buffer[MAX_CMDLINE];
static char* s_argv[MAX_ARGS];
static int   s_argc = -1; /* -1 indicates "not parsed yet" */

static void do_parse(void) {
    struct dos_psp* psp = (struct dos_psp*)_dos_getpdb();
    s_argc = 0;
    char* b_ptr = s_buffer;

    /* Set argv[0]: Executable name from PDB */
    s_argv[s_argc++] = b_ptr;
    strncpy(b_ptr, (char*)psp->exe_name, 23);
    b_ptr[23] = '\0';
    b_ptr += strlen(b_ptr) + 1;

    /* Get command line from PDB (comline[0] is length, [1] is start of string) */
    unsigned char* comline = (unsigned char*)psp->comline;
    int len = comline[0];
    char* p = (char*)&comline[1];
    char* end = p + len;
    
    int in_quote = 0;
    char quote_char = '\0';

    while (p < end && s_argc < (MAX_ARGS - 1)) {
        /* Skip leading whitespace */
        while (p < end && (*p == ' ' || *p == '\t')) p++;
        if (p >= end || *p == '\0') break;

        /* Check for quotation start */
        if (*p == '"' || *p == '\'') {
            in_quote = 1;
            quote_char = *p++;
        } else {
            in_quote = 0;
        }
        
        s_argv[s_argc++] = b_ptr;

        /* Copy characters to internal buffer */
        while (p < end && *p != '\0') {
            if (!in_quote && (*p == ' ' || *p == '\t')) break;
            if (in_quote && *p == quote_char) {
                p++; /* consume closing quote */
                break;
            }

            /* Shift-JIS handling: copy 2 bytes if it's a Kanji 1st byte */
            if (iskanji(*p) && (p + 1 < end)) {
                *b_ptr++ = *p++; /* 1st byte */
                *b_ptr++ = *p++; /* 2nd byte */
            } else {
                *b_ptr++ = *p++; /* single byte char */
            }
            
            /* Buffer overflow protection */
            if (b_ptr >= &s_buffer[MAX_CMDLINE - 1]) break;
        }
        *b_ptr++ = '\0'; /* Null-terminate each argument */
    }
    s_argv[s_argc] = NULL; /* argv array must end with NULL */
}

/**
 * Get the number of command line arguments.
 * Parses the command line automatically on the first call.
 */
int cmdline_get_argc() {
    if (s_argc == -1) do_parse();
    return s_argc;
}

/**
 * Get the argument vector (argv).
 * Parses the command line automatically on the first call.
 */
char** cmdline_get_argv() {
    if (s_argc == -1) do_parse();
    return s_argv;
}
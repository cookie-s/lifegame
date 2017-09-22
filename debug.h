#ifndef DEBUG_H
#define DEBUG_H

extern EFI_SYSTEM_TABLE *gST;
extern EFI_BOOT_SERVICES *gBS;

void putchar(unsigned char c) {
    char s[4] = {};
    s[0] = c;
    gST->ConOut->OutputString(gST->ConOut, s);
    return;
}

void print_uint(unsigned int u) {
    char s[50];

    if(u == 0) {
        putchar('0');
        return;
    }

    int i=0;
    for(; u; u/=10, i++)
        s[i] = '0' + (u % 10);
    for(; i--;)
        putchar(s[i]);
    return;
}

void print(const char *s) {
    while(*s)
        putchar(*s++);
    return;
}

void puts(const char *s) {
    print(s);
    putchar('\r');
    putchar('\n');
    return;
}

#endif

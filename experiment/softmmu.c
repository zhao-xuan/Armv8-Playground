#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_get_addr_info 451
long get_addr_info(void * addr) {
    return syscall(__NR_get_addr_info, addr);
}

int main(void) {
    char *s = malloc(sizeof(char) * 6);
    s[0] = 'h';
    s[1] = 'e';
    s[2] = 'l';
    s[3] = 'l';
    s[4] = 'o';
    s[5] = '\0';
    unsigned long p = get_addr_info((void *) s) & 0xffffffffffff;

    printf("The string is at 0x%016lx\n", (unsigned long) s);
    printf("The address produced by softmmu walk is 0x%016lx\n", (unsigned long) p);
    printf("The content retrieved by address produced by softmmu is %s\n", (char *) p);

    return 0;
}

#include <stdio.h>
#include <signal.h>

int main(void) {
    printf("This is the parent starting\n");
    raise(SIGUSR1);
}

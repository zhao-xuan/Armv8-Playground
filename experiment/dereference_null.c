#include <unistd.h>

int main(void) {
    int *a = (void*)0x12345;
    sleep(5);
    return *a;
}

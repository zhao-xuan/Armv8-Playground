#include <unistd.h>

int main(void) {
    int *a = (void *) 0xffff123456789101;
    sleep(5);
    return *a;
}

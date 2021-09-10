#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_get_el0_exception_count 449
#define __NR_get_el1_exception_count 450

long get_el0_exception_count(void)
{
    return syscall(__NR_get_el0_exception_count);
}

long get_el1_exception_count(void)
{
    return syscall(__NR_get_el1_exception_count);
}

int main(void) {
    printf("The number of exceptions coming from EL0: %ld\n", get_el0_exception_count());
    printf("The number of exceptions coming from EL1: %ld\n", get_el1_exception_count());
    return 0;
}

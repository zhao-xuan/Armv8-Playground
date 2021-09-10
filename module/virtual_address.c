#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <asm/sysreg.h>
#include <asm/memory.h>

MODULE_LICENSE("some license");

// The EL1 translation table base address is at TTBR1_EL1[47:1]
#define TTBR_BADDR_MASK     (UL(0x7fffffffffff) << 1)

int init_module(void) {
    unsigned long ttbr1 = read_sysreg(ttbr1_el1);
    unsigned long ttbr_addr = (((ttbr1 >> 1) & TTBR_BADDR_MASK) | (UL(0xffff) << 48));
    char *chunk = vmalloc(sizeof(char) * 4096);
    char *s = vmalloc(sizeof(char) * 5);
    unsigned long p = __pa(s);

    printk("The EL1 Translation Table is located at: 0x%016lx\n", ttbr_addr);
    
    s[0] = 'h';
    s[1] = 'e';
    s[2] = 'l';
    s[3] = 'l';
    s[4] = 'o';
    printk(KERN_INFO "Virtual address of string: 0x%016lx\n", (unsigned long) s);
    printk(KERN_INFO "Physcial address of string: 0x%016lx\n", p);
    printk(KERN_INFO "Virtual address of chunk: 0x%016lx\n", (unsigned long) chunk);
    printk(KERN_INFO "Physcial address of chunk: 0x%016llx\n", __pa(chunk));
    printk(KERN_INFO "The string is: %s\n", s);
    printk(KERN_INFO "The virtual address is, again: 0x%016lx\n", (unsigned long) __va(p));
    printk(KERN_INFO "Using the new virtual address to access the string: %s\n", (char *) __va(p));
    return 0;
}

void cleaup_module(void) {
    printk(KERN_INFO "Clean up!\n");
}
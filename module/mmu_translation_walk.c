#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <asm/sysreg.h>
#include <asm/memory.h>

MODULE_LICENSE("some license");

// The EL1 translation table base address is at TTBR1_EL1[47:12] when page size is 4KB
#define TTBR_BADDR_MASK     (UL(0xfffffffff) << 12)

#define TT_TABLE_DESC_MASK          (UL(0xfffffffff) << 12)
#define TT_PAGE_DESC_MASK           (UL(0xfffffffff) << 12)

#define VA_L0_INDEX(x)              (((((unsigned long) x) >> 39) & UL(0x1ff)) * 8)
#define VA_L1_INDEX(x)              (((((unsigned long) x) >> 30) & UL(0x1ff)) * 8)
#define VA_L2_INDEX(x)              (((((unsigned long) x) >> 21) & UL(0x1ff)) * 8)
#define VA_L3_INDEX(x)              (((((unsigned long) x) >> 12) & UL(0x1ff)) * 8)
#define VA_BITS_FIELD(x)                  (((unsigned long) x) & UL(0xfff))

int init_module(void) {
    unsigned long ttbr1 = read_sysreg(ttbr1_el1);
    unsigned long ttbr_addr = (ttbr1 & TTBR_BADDR_MASK);
    void *p = __va(ttbr_addr);
    char *s = vmalloc(sizeof(char) * 6);
    s[0] = 'h';
    s[1] = 'e';
    s[2] = 'l';
    s[3] = 'l';
    s[4] = 'o';
    s[5] = '\0';

    printk("The value in TTBR1_EL1 is 0x%016lx\n", ttbr1);
    printk("The L0 translation table is located at 0x%016lx\n", ttbr_addr);
    printk("The first entry of L0 translation table is located at physical address 0x%016lx\n", ttbr_addr);
    printk("The virtual location is 0x%016lx\n\n", (unsigned long) p);

    unsigned long l0 = *((unsigned long *) p);
    printk("Now you see L0 first entry 0x%016lx\n", l0);
    unsigned long l1 = *((unsigned long *) (__va(l0 & TT_TABLE_DESC_MASK)));
    printk("Now you see L1 first entry 0x%016lx\n", l1);
    unsigned long l2 = *((unsigned long *) (__va(l1 & TT_TABLE_DESC_MASK)));
    printk("Now you see L2 first entry 0x%016lx\n", l2);
    unsigned long l3 = *((unsigned long *) (__va(l2 & TT_TABLE_DESC_MASK)));
    printk("Now you see L3 first entry 0x%016lx\n", l3);
    unsigned long content = *((unsigned long *) (__va(l3 & TT_PAGE_DESC_MASK)));
    printk("Now you see the content that the L3 first entry is pointing to 0x%016lx\n\n", content);

    printk("The string is at address 0x%016lx", (unsigned long) s);
    unsigned long entry_l0 = *((unsigned long *) (p + VA_L0_INDEX(s)));
    printk("The L0 entry in the translation walk is 0x%016lx", entry_l0);
    unsigned long entry_l1 = *((unsigned long *) (__va((entry_l0 & TT_TABLE_DESC_MASK) + VA_L1_INDEX(s))));
    printk("The L1 entry in the translation walk is 0x%016lx", entry_l1);
    unsigned long entry_l2 = *((unsigned long *) (__va((entry_l1 & TT_TABLE_DESC_MASK) + VA_L2_INDEX(s))));
    printk("The L2 entry in the translation walk is 0x%016lx", entry_l2);
    unsigned long entry_l3 = *((unsigned long *) (__va((entry_l2 & TT_TABLE_DESC_MASK) + VA_L3_INDEX(s))));
    printk("The L3 entry in the translation walk is 0x%016lx", entry_l3);

    char *verify = ((char *) (__va((entry_l3 & TT_PAGE_DESC_MASK) + VA_BITS_FIELD(s))));
    printk("The string content: %s\n", verify);

    return 0;
}

void cleaup_module(void) {
    printk(KERN_INFO "Clean up!\n");
}
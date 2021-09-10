#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <asm/sysreg.h>
#include <asm/memory.h>
#include <asm/sysreg.h>

MODULE_LICENSE("some license");

// This module will explore both the attributes of table descriptors and page descriptors

// The EL1 translation table base address is at TTBR1_EL1[47:12] when page size is 4KB
#define TTBR_BADDR_MASK     (UL(0xfffffffff) << 12)

// table and page descriptor address mask
#define TT_TABLE_DESC_ADDR_MASK          (UL(0xfffffffff) << 12)
#define TT_PAGE_DESC_ADDR_MASK           (UL(0xfffffffff) << 12)

// table descriptor attributes
#define TT_TABLE_NS_BIT(x)          ((((unsigned long) x) >> 63) & UL(0x1))
#define TT_TABLE_AP_BIT(x)          ((((unsigned long) x) >> 61) & UL(0x3))
#define TT_TABLE_XN_BIT(x)          ((((unsigned long) x) >> 60) & UL(0x1))
#define TT_TABLE_PXN_BIT(x)         ((((unsigned long) x) >> 59) & UL(0x1))

// the index to the translation table at different levels in a virtual address
#define VA_L0_INDEX(x)              (((((unsigned long) x) >> 39) & UL(0x1ff)) * 8)
#define VA_L1_INDEX(x)              (((((unsigned long) x) >> 30) & UL(0x1ff)) * 8)
#define VA_L2_INDEX(x)              (((((unsigned long) x) >> 21) & UL(0x1ff)) * 8)
#define VA_L3_INDEX(x)              (((((unsigned long) x) >> 12) & UL(0x1ff)) * 8)
#define VA_BITS_FIELD(x)            (((unsigned long) x) & UL(0xfff))

// page descriptor attributes
#define TT_PAGE_CONT(x)             ((((unsigned long) x) >> 52) & UL(0x1))
#define TT_PAGE_DBM(x)              ((((unsigned long) x) >> 51) & UL(0x1))
#define TT_PAGE_nG(x)               ((((unsigned long) x) >> 11) & UL(0x1))
#define TT_PAGE_AF(x)               ((((unsigned long) x) >> 10) & UL(0x1))
#define TT_PAGE_SH(x)               ((((unsigned long) x) >> 8) & UL(0x3))
#define TT_PAGE_AP(x)               ((((unsigned long) x) >> 6) & UL(0x3))
#define TT_PAGE_NS(x)               ((((unsigned long) x) >> 5) & UL(0x1))
#define TT_PAGE_ATTR(x)             ((((unsigned long) x) >> 2) & UL(0x7))

void print_page_desc_attr(unsigned long addr) {
    char *cont = TT_PAGE_CONT(addr) ? "contiguous s" : "non-contiguous";
    char *dirt = TT_PAGE_DBM(addr) ? "dirty" : "NOT dirty";
    char *nG   = TT_PAGE_nG(addr) ? "not global" : "global";
    char *af   = TT_PAGE_AF(addr) ? "been accessed" : "not been accessed yet";
    char *sh[4] = {"non-shareable", "CONSTRAINED UNPREDICTABLE", "outer shareable", "inner shareable"};
    char *ap[4] = {"read/write from privileged EL", "read/write from EL0", "read-only from privileged EL", "read-only from EL0"};
    char *ns = TT_PAGE_NS(addr) ? "secure access" : "non-secure access";
    char **attr = vmalloc(sizeof(char *) * 0x100);

    attr[MAIR_ATTR_DEVICE_nGnRnE] = "nGnRnE device memory";
    attr[MAIR_ATTR_DEVICE_nGnRE] = "nGnRE device memory";
    attr[MAIR_ATTR_NORMAL_NC] = "non-sharable normal memory";
    attr[MAIR_ATTR_NORMAL_TAGGED] = "tagged normal memory";
    attr[MAIR_ATTR_NORMAL] = "normal memory";

    unsigned long mair = read_sysreg(mair_el1);
    unsigned long attr_val = (mair >> TT_PAGE_ATTR(addr)) & 0xff;

    printk("\n\n\n*****************Table descriptor attribute********************\n");
    printk("Memory address: 0x%016lx", addr);
    printk("The page belongs to a %s sequence of pages\n", cont);
    printk("The page is %s\n", dirt);
    printk("The page is %s\n", nG);
    printk("The page has %s\n", af);
    printk("The shareability of this page is %s\n", sh[TT_PAGE_SH(addr)]);
    printk("The access rights of this page is %s\n", ap[TT_PAGE_AP(addr)]);
    printk("The memory is a %s", ns);
    printk("The memory type is %s\n", attr[attr_val]);
    printk("*****************Table descriptor attribute********************\n\n\n");

    vfree(attr);
}

void print_table_desc_attr(unsigned long entry, int level) {
    char *ns = TT_TABLE_NS_BIT(entry) ? "secure" : "non-secure";
    char *ap[4] = {"read/write from privileged EL", "read/write from EL0", "read-only from privileged EL", "read-only from EL0"};
    char *xn = TT_TABLE_XN_BIT(entry) ? "EL0 cannot execute" : "EL0 can execute";
    char *pxn = TT_TABLE_PXN_BIT(entry) ? "priviledged execute never" : "priviledged level can execute";

    printk("\n\n\n#################Table descriptor attribute###################\n");
    printk("This L%d entry demands %s memory access in the subsequent level\n", level, ns);
    printk("This L%d entry has access rights: %s\n", level, ap[TT_TABLE_AP_BIT(entry)]);
    printk("This L%d entry has user execute right: %s\n", level, xn);
    printk("This L%d entry has priviledged level execute right: %s\n", level, pxn);
    printk("#################Table descriptor attribute###################\n\n\n");
}

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

    printk("The string is at address 0x%016lx", (unsigned long) s);

    unsigned long entry_l0 = *((unsigned long *) (p + VA_L0_INDEX(s)));
    unsigned long entry_l1 = *((unsigned long *) (__va((entry_l0 & TT_TABLE_DESC_ADDR_MASK) + VA_L1_INDEX(s))));
    unsigned long entry_l2 = *((unsigned long *) (__va((entry_l1 & TT_TABLE_DESC_ADDR_MASK) + VA_L2_INDEX(s))));
    unsigned long entry_l3 = *((unsigned long *) (__va((entry_l2 & TT_TABLE_DESC_ADDR_MASK) + VA_L3_INDEX(s))));
    char *verify = ((char *) (__va((entry_l3 & TT_PAGE_DESC_ADDR_MASK) + VA_BITS_FIELD(s))));

    printk("The L0 entry in the translation walk is 0x%016lx", entry_l0);
    print_table_desc_attr(entry_l0, 0);

    printk("The L1 entry in the translation walk is 0x%016lx", entry_l1);
    print_table_desc_attr(entry_l1, 1);

    printk("The L2 entry in the translation walk is 0x%016lx", entry_l2);
    print_table_desc_attr(entry_l2, 2);

    printk("The L3 entry in the translation walk is 0x%016lx", entry_l3);
    print_page_desc_attr(entry_l3);

    printk("The string content: %s\n", verify);

    return 0;
}

void cleaup_module(void) {
    printk(KERN_INFO "Clean up!\n");
}
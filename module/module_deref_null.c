#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("soem license");

int init_module(void) {
    // Should see a permission fault here
    int *a = (void *) 0xffff123456789101; // if change to 0x0 then should see a EL0 translation fault here
    printk(KERN_INFO "About to blow up!\n");
    printk(KERN_INFO "Should I see this line?\n");
    return *a;
}

void cleanup_module(void) {
    printk(KERN_INFO "cleaup!\n");
}

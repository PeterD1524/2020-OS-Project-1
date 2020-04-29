#include <linux/ktime.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
asmlinkage void GETNSTIMEOFDAY(struct timespec* ts) {
    struct timespec* TS = kmalloc(sizeof(struct timespec), GFP_KERNEL);
    getnstimeofday(TS);
    copy_to_user(ts, TS, sizeof(struct timespec));
}
